#pragma once
#include <mpch.h>

#include "Mana/Math/Crypto.h" // Needed for using M_ASSET()

#include <typeindex>

namespace Mana {
class ServicesManager;
// Macro for asset that hashes the asset id at compile time
#define M_ASSETNAME(name) ::Mana::Crypto::HashC_fnv1a(name)
#define M_ASSET(type, name)                                                    \
    ::Mana::Application::Instance()->GetAssets().GetAsset<type>(               \
        M_ASSETNAME(name))

struct MoteHeader {
    uint32_t Magic;
    uint32_t Version;
    uint8_t AssetType;
    uint32_t NumChunks;
};

struct MoteChunkHeader {
    uint8_t Type;
    size_t Size;
    size_t OriginalSize;
};

struct MoteChunk {
    const MoteChunkHeader *Header;
    std::span<const uint8_t> Data;
};

struct MoteChunkOwned {
    MoteChunkHeader Header;
    std::vector<uint8_t> Data;
};

struct MoteData {
    const MoteHeader *Header;
    std::vector<MoteChunk> Chunks;
};

class Motes {
  public:
    static MoteChunkOwned CompressDataToChunk(uint8_t type,
                                              const std::vector<uint8_t> &data);
    static MoteChunkOwned CompressDataToChunk(uint8_t type, const uint8_t *data,
                                              const size_t size);

    static std::vector<uint8_t>
    ReadCompressedBinaryData(const MoteChunk &chunk);

    template <typename T>
    static MoteChunkOwned CompressDataToChunk(uint8_t type,
                                              std::vector<T> &data) {
        return CompressDataToChunk(
            type, reinterpret_cast<const uint8_t *>(data.data()),
            data.size() * sizeof(T));
    }

    template <typename T>
    static std::vector<T> ReadCompressedBinaryData(const MoteChunk &chunk,
                                                   const uint count) {
        std::vector<uint8_t> rawData = ReadCompressedBinaryData(chunk);

        // Calculate expected size and validate
        const size_t expectedSize = count * sizeof(T);
        M_ASSERT(rawData.size() == expectedSize);

        // Create vector and copy data
        std::vector<T> result(count);
        std::memcpy(result.data(), rawData.data(), rawData.size());

        return result;
    }

    template <typename T>
    static MoteChunkOwned StructToChunk(uint8_t type, T data) {
        MoteChunkOwned chunk;
        chunk.Header.Size = sizeof(T);
        chunk.Header.OriginalSize = sizeof(T);
        chunk.Header.Type = type;
        chunk.Data.resize(sizeof(T));
        std::memcpy(chunk.Data.data(), &data, sizeof(T));
        return chunk;
    }

    template <typename T> static const T *ChunkToStruct(MoteChunk chunk) {
        if (chunk.Header->Size != sizeof(T)) {
            MC_ERROR("Unable to read chunk as a struct!!");
            M_ASSERT(false);
            return nullptr;
        }

        return reinterpret_cast<const T *>(chunk.Data.data());
    }
};

enum class AssetType : uint8_t {
    Other = 0,
    Shader,
    Texture,
    Sound,
    Font,
    SpriteSheet
};

inline const char *AssetTypeToStr(const AssetType type) {
    switch (type) {
    case AssetType::Other:
        return "Other / Unknown";
    case AssetType::Shader:
        return "Shader";
    case AssetType::Texture:
        return "Texture";
    case AssetType::Font:
        return "Font";
    case AssetType::Sound:
        return "Sound";
    case AssetType::SpriteSheet:
        return "SpriteSheet";
    }
}

// Mote File Layout (v2)
// 1. MoteHeader (fixed size)
// 2-n. Chunks (number of chunks from header, chunk sizes at start of chunks)
//
// The AssetVersion is the timestamp that the asset file the mote was based on
// was created. (Stored in .motes)
// The ID is the id of the mote file read from the filename
struct MoteFile {
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>
        AssetVersion;
    uint64_t Hash;
    AssetType Type;
};

// The mote is populated with the header and the chunks decoded from the data
// given, therefore if the data is unloaded from memory so is the motefile
MoteData LoadMoteFileData(const std::vector<uint8_t> &data);

class TextFile {
  public:
    TextFile(const std::vector<uint8_t> &data);
    std::string &Text() { return m_Text; };

    static Ref<TextFile> Create(const std::vector<uint8_t> &data) {
        return CreateRef<TextFile>(data);
    };

  private:
    std::string m_Text;
};

class AssetManager {
  public:
    AssetManager(std::string cacheFolder,
                 std::vector<std::filesystem::path> sourceFolders,
                 Scope<ServicesManager> &services);
    void CompileNewAssets();
    void RunAssetGarbageCollection();
    void ReloadAssetMap();
    AssetType BuildMote(const uint64_t id, std::filesystem::path mote,
                        std::filesystem::path src);

    template <typename T> Ref<T> GetAsset(const uint64_t &hash) {
        auto &assetMap = m_Assets[typeid(T)]; // Get map for type T
        auto it = assetMap.find(hash);

        if (it != assetMap.end()) {
            // Return the cached asset, casted to the correct type
            return std::static_pointer_cast<T>(it->second);
        }

        // If asset not found, load it
        Ref<T> asset = LoadAsset<T>(hash);
        assetMap[hash] = asset;
        return asset;
    }

    template <typename T> void ReloadIfExists(const uint64_t &hash) {
        auto &assetMap = m_Assets[typeid(T)]; // Get map for type T
        auto it = assetMap.find(hash);

        if (it != assetMap.end()) {
            MC_TRACE("Asset was loaded, hotswapping");
            auto mote = m_MoteFiles[hash];
            auto motePath =
                m_FolderCache / (std::to_string(mote.Hash) + ".mote");
            auto data = Utils::Files::loadBinaryFromFile(motePath);
            auto moteData = LoadMoteFileData(data);

            // TODO: RELOAD THE ASSET DATA
            //            ((T *)it->second)->Reload(moteData);
        }
    }

    template <typename T>
    std::vector<std::shared_ptr<T>>
    GetAssetsByType(bool loadAll = false, AssetType type = AssetType::Other) {
        if (loadAll) {
            std::vector<std::shared_ptr<T>> out;
            for (auto &moteFile : m_MoteFiles) {
                if (moteFile.second.Type == type)
                    out.push_back(GetAsset<T>(moteFile.first));
            }

            return out;
        } else {
            std::vector<std::shared_ptr<T>> out;

            for (auto &asset : m_Assets[typeid(T)]) {
                out.push_back(GetAsset<T>(asset.first));
            }

            return out;
        }
    }

    void EnsureDestructors() { m_PendingDestruction.clear(); }

  private:
    template <typename T> Ref<T> LoadAsset(const uint64_t &hash) {
        auto mote = m_MoteFiles[hash];
        if (!mote.Hash) {
            MC_ERROR("Mote with not found, returning nullptr. Search hash:{}",
                     hash);
            return nullptr;
        }

        auto motePath = m_FolderCache / (std::to_string(mote.Hash) + ".mote");
        auto data = Utils::Files::loadBinaryFromFile(motePath);

        if (data.empty()) {
            MC_ERROR("Mote file data is null, returning nullptr");
            return nullptr;
        }

        auto moteData = LoadMoteFileData(data);

        return T::Create(moteData);
    }

  private:
    std::filesystem::path m_FolderCache, m_MotesFile;
    std::vector<std::filesystem::path> m_SourceFolders;
    std::map<uint64_t, MoteFile> m_MoteFiles;

    std::unordered_map<std::type_index,
                       std::unordered_map<uint64_t, std::shared_ptr<void>>>
        m_Assets;
    std::mutex m_DestructionMutex;
    std::vector<std::function<void()>> m_PendingDestruction;
};

} // namespace Mana
