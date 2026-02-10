#include <fstream>
#include <sstream>

#include "Mana/Core/Application.h"

#include "SpriteSheet.h"

namespace Mana {
std::vector<uint8_t>
SpriteSheet::SpriteToBinary(const uint8_t id, SpriteSheet::ShtSprite sprite) {
    std::vector<uint8_t> out;
    Utils::Files::BufferWriter writer(out);
    writer.Write<uint8_t>(id);
    writer.Write<uint>(sprite.Pos.x);
    writer.Write<uint>(sprite.Pos.y);
    writer.Write<uint>(sprite.Size.x);
    writer.Write<uint>(sprite.Size.y);
    writer.WriteSized(sprite.Name);
    return out;
}

SpriteSheet::ShtSprite SpriteSheet::BinaryToSprite(uint8_t &id,
                                                   std::vector<uint8_t> data) {
    Utils::Files::BufferReader reader(data);
    ShtSprite out;
    id = reader.Read<uint8_t>();
    out.Pos.x = reader.Read<uint>();
    out.Pos.y = reader.Read<uint>();
    out.Size.x = reader.Read<uint>();
    out.Size.y = reader.Read<uint>();
    out.Name = reader.ReadNextString();

    return out;
}

void SpriteSheet::CreateMote(std::filesystem::path src,
                             std::vector<MoteChunkOwned> &data) {
    std::ifstream infile(src);
    if (!infile) {
        MC_ERROR("Failed to read file at: {}", src.c_str());
        return;
    }

    // Read line by line
    std::string line, key;
    while (std::getline(infile, line)) {
        size_t split = line.find(':');
        if (split == std::string::npos) {
            MC_WARN("Failed to find delimeter, skipping line: \"{}\"", line);
            continue;
        }

        key =
            line.substr(0, split); // Either "Texture" or the name of the Sprite
        line.erase(
            0,
            split + 1); // Either a texture name or a value like "128 0 0 24 24"

        if (key == "Texture") {
            auto textureHashId = M_ASSETNAME(line);
            data.push_back(Motes::StructToChunk<MetaData>(ChunkType::Metadata,
                                                          {textureHashId}));
        } else {
            uint id; // This can't be uint8_t sadly, as it would read the id as
                     // ascii characets and give incorrect values
            ShtSprite sprite;
            sprite.Name = key;

            std::stringstream ss(line);
            ss >> id >> sprite.Pos.x >> sprite.Pos.y >> sprite.Size.x >>
                sprite.Size.y;

            if (id == 0) {
                MC_WARN("The sprite {}, has id=0, which is not supported!",
                        sprite.Name);
            }

            data.push_back(Motes::CompressDataToChunk(
                ChunkType::Sprite,
                SpriteToBinary(static_cast<uint8_t>(id), sprite)));
        }
    }
}
Ref<SpriteSheet> SpriteSheet::Create(const MoteData &data) {
    uint64_t textureHashId = 0;
    std::map<uint8_t, ShtSprite> sprites;

    for (auto c : data.Chunks) {
        if (c.Header->Type == ChunkType::Sprite) {
            uint8_t id;
            ShtSprite sprite =
                BinaryToSprite(id, Motes::ReadCompressedBinaryData(c));
            sprites[id] = sprite;
        } else if (c.Header->Type == ChunkType::Metadata) {
            const auto metaData = Motes::ChunkToStruct<MetaData>(c);
            textureHashId = metaData->TextureHashID;
        } else {
            MC_WARN("Invalid ChunkType found ({}), skipping chunk.",
                    c.Header->Type);
        }
    }

    if (textureHashId == 0) {
        MC_WARN("Found no / invalid Metadata chunk, returning nullptr");
        return nullptr;
    }

    return CreateRef<SpriteSheet>(
        ::Mana::Application::Instance()->GetAssets().GetAsset<Texture>(
            textureHashId),
        std::move(sprites));
}

SpriteSheet::SpriteSheet(Ref<Texture> texture,
                         std::map<uint8_t, ShtSprite> &&sprites)
    : m_Texture(texture), m_Sprites(std::move(sprites)) {
    // We need to calculate the texture coords for each sprite
    const uint32_t width = m_Texture->GetWidth();
    const uint32_t height = m_Texture->GetHeight();

    for (auto &sprite : m_Sprites) {
        float u0 = static_cast<float>(sprite.second.Pos.x) / width;
        float v0 = static_cast<float>(sprite.second.Pos.y) / height;
        float u1 =
            static_cast<float>(sprite.second.Pos.x + sprite.second.Size.x) /
            width;
        float v1 =
            static_cast<float>(sprite.second.Pos.y + sprite.second.Size.y) /
            height;
        sprite.second.TexCoords = {{
            {u0, v0}, // bottom-left
            {u1, v0}, // bottom-right
            {u1, v1}, // top-right
            {u0, v1}, // top-left
        }};
    }
}
} // namespace Mana
