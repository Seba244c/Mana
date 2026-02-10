#include "Asset.h"

#include "Mana/Audio/Sound.h"
#include "Mana/Core/Application.h"
#include "Mana/Core/Assert.h"
#include "Mana/Core/Log.h"
#include "Mana/Core/Utils.h"
#include "Mana/Graphics/Shader.h"
#include "Mana/Graphics/Texture.h"
#include "Mana/Rendering/Font.h"
#include "Mana/Rendering/SpriteSheet.h"
#include "Mana/Services/Service.h"

#include <lz4.h>
#include <lz4hc.h>

namespace Mana {
static uint32_t MoteHeaderMagicNumber = 4206942069;
static uint8_t MoteFormatVersion = 2;

std::string AtToStr(const AssetType value) {
    return std::string(1, static_cast<char>(value));
}

// Convert string back to enum
AssetType StrToAt(const std::string &str) {
    M_ASSERT(!str.empty());
    return static_cast<AssetType>(static_cast<uint8_t>(str[0]));
}

MoteData LoadMoteFileData(const std::vector<uint8_t> &data) {
    // Read MoteHeader
    if (data.size() < sizeof(MoteHeader)) { // Smallets possible mote
        M_ASSERT(false, "Mote is too small to contain header!")
        return {};
    }

    const MoteHeader *header =
        reinterpret_cast<const MoteHeader *>(data.data());

    // Verification
    M_ASSERT(header->Magic == MoteHeaderMagicNumber);
    if (header->Version != MoteFormatVersion)
        MC_WARN("Mote is in an incompatible version: v{} instead of v{}",
                header->Version, MoteFormatVersion);

    // Construct mote data
    size_t position = sizeof(MoteHeader);
    std::vector<MoteChunk> Chunks;

    for (int i = 0; i < header->NumChunks; i++) {
        const MoteChunkHeader *chunkHeader =
            reinterpret_cast<const MoteChunkHeader *>(data.data() + position);
        position += sizeof(MoteChunkHeader);

        if (position + chunkHeader->Size > data.size()) {
            M_ASSERT(false, "Incomplete chunk!");
            MC_WARN("Skipped chunk with incomplete data");
            break;
        }

        Chunks.push_back(MoteChunk{
            chunkHeader, {data.data() + position, chunkHeader->Size}});

        position += chunkHeader->Size;
    }

    return {header, Chunks};
}
MoteChunkOwned Motes::CompressDataToChunk(uint8_t type,
                                          const std::vector<uint8_t> &data) {
    const auto bounds = LZ4_compressBound(data.size());
    std::vector<uint8_t> compressedData(bounds);

    const int compressedSize = LZ4_compress_HC(
        reinterpret_cast<const char *>(data.data()),
        reinterpret_cast<char *>(compressedData.data()), data.size(), bounds,
        9); // Using default compression level 9
    M_ASSERT(compressedSize > 0);
    compressedData.resize(compressedSize);

    return MoteChunkOwned{
        MoteChunkHeader{type, static_cast<size_t>(compressedSize), data.size()},
        compressedData};
}

MoteChunkOwned Motes::CompressDataToChunk(uint8_t type, const uint8_t *data,
                                          const size_t size) {
    const auto bounds = LZ4_compressBound(size);
    std::vector<uint8_t> compressedData(bounds);

    const int compressedSize = LZ4_compress_HC(
        reinterpret_cast<const char *>(data),
        reinterpret_cast<char *>(compressedData.data()), size, bounds,
        9); // Using default compression level 9
    M_ASSERT(compressedSize > 0);
    compressedData.resize(compressedSize);

    return MoteChunkOwned{
        MoteChunkHeader{type, static_cast<size_t>(compressedSize), size},
        compressedData};
}

std::vector<uint8_t> Motes::ReadCompressedBinaryData(const MoteChunk &chunk) {
    std::vector<uint8_t> data(chunk.Header->OriginalSize);
    const int decompressed_size = LZ4_decompress_safe(
        reinterpret_cast<const char *>(chunk.Data.data()),
        reinterpret_cast<char *>(data.data()), (int)chunk.Header->Size,
        (int)chunk.Header->OriginalSize);
    M_ASSERT(decompressed_size == chunk.Header->OriginalSize);
    return data;
}

AssetManager::AssetManager(std::string cachePath,
                           std::vector<std::filesystem::path> sourcePaths,
                           Scope<ServicesManager> &services)
    : m_FolderCache(cachePath), m_SourceFolders(sourcePaths) {
    // Make sure folders exists
    if (!std::filesystem::exists(m_FolderCache)) {
        MC_TRACE("Cache folder not found. Creating cache folder...");
        M_ASSERT(std::filesystem::create_directory(m_FolderCache),
                 "Failed to create asset cache folder");
    }
    MC_TRACE("Cache Folder: {}",
             std::filesystem::canonical(m_FolderCache).c_str());

    for (auto &sourceFolder : m_SourceFolders) {
        if (!std::filesystem::exists(sourceFolder)) {
            MC_WARN("Assets folder \"{}\" not found. Creating new empty folder",
                    sourceFolder.c_str());
            M_ASSERT(std::filesystem::create_directory(sourceFolder),
                     "Failed to create asset source folder");
        }
        MC_TRACE("Source Folder: {}",
                 std::filesystem::canonical(sourceFolder).c_str());
    }

    // Load list of existing motes
    m_MotesFile = m_FolderCache / ".motes";
    if (!std::filesystem::exists(m_MotesFile))
        MC_WARN("No motes file found! Loading 0 assets");
    else
        ReloadAssetMap();

    // Asset Garbage Collection
    if (Application::Instance()->GetConfig().Assets_GarbageCollection)
        services->Schedule(
            "assetsGarbageCollection",
            {[this]() { RunAssetGarbageCollection(); }, JobPriority::Normal},
            Time::Seconds(8.6));

// Asset Runtime Compile
#ifdef M_DEBUG
    CompileNewAssets();
    if (Application::Instance()->GetConfig().HotCompileAssets)
        services->Schedule("assetsRuntimeCompile",
                           {[this]() { CompileNewAssets(); }, JobPriority::Low},
                           Time::Seconds(8));
#endif
}

AssetType AssetTypeFromExtension(std::string extension) {
    if (extension == ".glsl") {
        return AssetType::Shader;
    } else if (extension == ".ttc" || extension == ".ttf") {
        return AssetType::Font;
    } else if (extension == ".png" || extension == ".jpg") {
        return AssetType::Texture;
    } else if (extension == ".sht") {
        return AssetType::SpriteSheet;
    } else if (extension == ".mp3" || extension == ".ogg") {
        return AssetType::Sound;
    }

    MC_WARN(" - Unkown file-extension, creating as AssetType::Other");
    return AssetType::Other;
}

void AssetManager::ReloadAssetMap() {
    // Creates map of motes
    MC_TRACE("Loading asset map from .motes file");
    std::ifstream motesFile(m_FolderCache / ".motes");

    std::string line;
    while (std::getline(motesFile, line)) {
        auto split = Utils::String::splitString(line, '#');
        M_ASSERT(split.size() == 3, "Incorrectly formatted motes file");
        MC_TRACE(" - Found mote with id {}, version {}, type {}", split[0],
                 split[1], split[2]); // TODO: Type not printing properly

        // Parse
        auto id = std::stoull(split[0]);
        // TODO: STORE AS UINT
        auto version = Utils::Time::TimeFromString(split[1]);
        AssetType type = StrToAt(split[2]);

        // Add to map
        m_MoteFiles[id] = {version, id, type};
    }
}

AssetType AssetManager::BuildMote(const uint64_t id, std::filesystem::path mote,
                                  std::filesystem::path src) {
    // Construct mote header
    MoteHeader header{MoteHeaderMagicNumber, 2, 0, 0};
    std::vector<MoteChunkOwned> data;
    AssetType assetType = AssetTypeFromExtension(src.extension());
    header.AssetType = (uint8_t)assetType;

    switch (assetType) {
    case AssetType::Shader:
        MC_TRACE(" - Creating as AssetType::Shader");
        Shader::CreateMote(src, mote, data);
        break;
    case AssetType::Texture:
        MC_TRACE(" - Creating as AssetType::Texture");
        Texture::CreateMote(src, data);
        break;
    case AssetType::Sound:
        MC_TRACE(" - Creating as AssetType::Sound");
        Sound::CreateMote(src, data);
        break;
    case AssetType::Font:
        MC_TRACE(" - Creating as AssetType::Font");
        Font::CreateMote(src, data);
        break;
    case AssetType::SpriteSheet:
        MC_TRACE(" - Creating as AssetType::SpriteSheet");
        SpriteSheet::CreateMote(src, data);
        break;
    case AssetType::Other:
        MC_WARN(" - Unkown file-extension, creating as AssetType::Other");
        auto otherData = Utils::Files::loadBinaryFromFile(src);
        data.push_back(Motes::CompressDataToChunk(0, otherData));
        break;
    }

    header.NumChunks = data.size();
    header.AssetType = (uint8_t)assetType;

    MC_TRACE(" - Writing to mote {} with {} chunks", mote.string(),
             header.NumChunks);
    std::ofstream out(mote);
    Utils::Files::Write<MoteHeader>(out, header);
    // Write Chunks
    for (const auto &chunk : data) {
        Utils::Files::Write<MoteChunkHeader>(out, chunk.Header);
        if (!chunk.Data.empty())
            out.write(reinterpret_cast<const char *>(chunk.Data.data()),
                      chunk.Data.size());
    }

    out.close();

// Reload if exists
#ifdef M_DEBUG
    if (assetType == AssetType::Shader)
        ReloadIfExists<Shader>(id);
    else if (assetType == AssetType::Texture)
        ReloadIfExists<Texture>(id);
#endif

    return assetType;
}

void AssetManager::RunAssetGarbageCollection() {
    unsigned int loaded = 0;

    for (auto &assetType : m_Assets) {
        std::erase_if(assetType.second, [this](const auto &pair) {
            if (pair.second.use_count() < 2) {
                MC_INFO("Asset {} has {} references, destroying...", pair.first,
                        pair.second.use_count());

                // We defer destroying the assets to the main thread, so that
                // OpenGL doesn't segfault
                // TODO: IMPLEMENT RENDERCOMMANDS ALLOWING FOR OPENGL COMMANDS
                // TO "RUN" ON OTHER THREADS
                std::shared_ptr<void> assetCopy =
                    pair.second; // Create a new reference
                std::lock_guard lock(m_DestructionMutex);
                m_PendingDestruction.emplace_back([assetCopy]() {});

                return true;
            }
            return false;
        });

        loaded += assetType.second.size();
    }

    MC_INFO("{} assets loaded", loaded);
}

void AssetManager::CompileNewAssets() {
    // Searches for new and updated motes, build and remakes .motes file
    MC_INFO("Searching for updated & new asset files...");

    uint32_t newMotes = 0;
    for (auto const &sourceFolder : m_SourceFolders) {
        for (auto const &dir_entry :
             std::filesystem::recursive_directory_iterator(sourceFolder)) {
            if (dir_entry.is_directory())
                continue;

            // Identify file
            uint64_t id = Mana::Crypto::Hash_fnv1a(
                dir_entry.path()
                    .string()
                    .substr(sourceFolder.string().length())
                    .c_str());
            auto parentFolder = dir_entry.path().parent_path().string() + "/";
            /* MC_TRACE("Found asset file: {} ({})", dir_entry.path().string(),
                     id); */

            // Skip asset if up to date
            // We convert the time to string and back to ensure the same
            // precision as the data in the motes file
            auto fileLastWrite = Utils::Time::TimeFromString(
                Utils::Time::TimeToString(dir_entry.last_write_time()));

            // If a mote exists, skip if they are the same version
            if (m_MoteFiles.contains(id)) {
                // Convert time to a common system time
                if (fileLastWrite <= m_MoteFiles[id].AssetVersion)
                    continue;
            }

            MC_TRACE("Asset was updated! building new mote! ({})", id);

            // "Build"
            const auto assetType =
                BuildMote(id, m_FolderCache / (std::to_string(id) + ".mote"),
                          dir_entry.path());

            // Add to cache
            auto mote = MoteFile{fileLastWrite, id, assetType};
            m_MoteFiles[id] = mote;
            newMotes++;
        }
    }

    // Update motes file
    if (newMotes < 1) {
        MC_TRACE("No new motes");
        return;
    }

    MC_INFO("Recreating motes file with {} new motes", newMotes);
    std::ofstream motesFile(m_FolderCache / ".motes",
                            std::ios::binary | std::ios::trunc);
    for (auto mote : m_MoteFiles) {
        // Write the motes name, file id, and version
        std::string moteDesc =
            std::format("{}#{}#{}\n", mote.first,
                        Utils::Time::TimeToString(mote.second.AssetVersion),
                        AtToStr(mote.second.Type));
        motesFile.write(moteDesc.c_str(), moteDesc.length());
    }
    motesFile.close();
}

TextFile::TextFile(const std::vector<uint8_t> &data) {
    m_Text = std::string(data.begin(), data.end());
}
} // namespace Mana
