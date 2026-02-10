#include <core/Bitmap.h>
#include <core/base.h>
#include <cstring>
#include <ext/import-font.h>
#include <glm/ext/vector_int2.hpp>

#include "Font.h"
#include "Mana/Core/Application.h"
#include "Mana/Core/Asset.h"
#include "Mana/Graphics/Texture.h"

#include "msdf-atlas-gen/AtlasGenerator.h"
#include "msdf-atlas-gen/BitmapAtlasStorage.h"
#include "msdf-atlas-gen/Charset.h"
#include "msdf-atlas-gen/FontGeometry.h"
#include "msdf-atlas-gen/GlyphGeometry.h"
#include "msdf-atlas-gen/GridAtlasPacker.h"
#include "msdf-atlas-gen/ImmediateAtlasGenerator.h"
#include "msdf-atlas-gen/TightAtlasPacker.h"
#include "msdf-atlas-gen/glyph-generators.h"
#include "msdf-atlas-gen/types.h"

namespace Mana {
static msdfgen::FreetypeHandle *s_FreeType;
static uint64_t coloringSeed = 0;
static constexpr unsigned long long lcgMultiplier = 6364136223846793005ull;

enum FontChunkType : uint8_t {
    TTFFile = 0,
    MetaData = 1,
};

static glm::ivec2
PackCharactersTightly(std::vector<msdf_atlas::GlyphGeometry> &glyphs) {
    glm::ivec2 out;
    // TightAtlasPacker class computes the layout of the atlas.
    msdf_atlas::TightAtlasPacker packer;
    packer.setDimensionsConstraint(msdf_atlas::DimensionsConstraint::SQUARE);
    // setScale for a fixed size or setMinimumScale to use the largest that fits
    packer.setMinimumScale(36.0);
    packer.setPixelRange(4);
    packer.setMiterLimit(1.0);

    // Compute atlas layout - pack glyphs
    int r = packer.pack(glyphs.data(), glyphs.size());
    M_ASSERT(r == 0, "Failed to pack all glyphs! Maybe there wasn't space?");

    // Get final atlas dimensions
    packer.getDimensions(out.x, out.y);
    return out;
}

static ImageData
CreateAtlas(const glm::ivec2 size,
            const std::vector<msdf_atlas::GlyphGeometry> &glyphs) {
    // Generate the atlas
    msdf_atlas::ImmediateAtlasGenerator<
        float, 3, msdf_atlas::msdfGenerator,
        msdf_atlas::BitmapAtlasStorage<uint8_t, 3>>
        generator(size.x, size.y);

    msdf_atlas::GeneratorAttributes attributes;
    attributes.scanlinePass = true;
    attributes.config.overlapSupport = true;
    generator.setAttributes(attributes);
    generator.setThreadCount(4);
    generator.generate(glyphs.data(), glyphs.size());

    // Export the atlas
    auto bitmap = (msdfgen::BitmapConstRef<uint8_t, 3>)generator.atlasStorage();
    const size_t dataSize = bitmap.width * bitmap.height * 3;
    ImageData imageData;
    imageData.Width = bitmap.width;
    imageData.Height = bitmap.height;
    imageData.Channels = 3;
    imageData.minifyFilter =
        FilteringMethod::LINEAR; // DO NOT CHANGE THIS!!! CHANGE TO NEAREST AND
                                 // YOU DIE!
    imageData.magnifyFilter = FilteringMethod::LINEAR;
    imageData.Data = std::vector<uint8_t>(dataSize);
    std::memcpy(imageData.Data.data(), bitmap.pixels, dataSize);
    return imageData;
}

void Font::CreateMote(std::filesystem::path src,
                      std::vector<MoteChunkOwned> &data) {
    // TODO: LOAD FONT AHEAD (we save a lot of space with bitmap etc)
    auto ttfFile = Utils::Files::loadBinaryFromFile(src);
    data.push_back(Motes::CompressDataToChunk(FontChunkType::TTFFile, ttfFile));

    // Metadata
    FontMetadata metadataChunk;
    metadataChunk.Charset = ASCII; // No other types as of know
    metadataChunk.SourceType = TTF;
    data.push_back(
        Motes::StructToChunk(FontChunkType::MetaData, metadataChunk));
}

Ref<Font> Font::Create(const MoteData &data) {
    // Initialize freetype if not already
    if (!s_FreeType) {
        MC_TRACE("Initializing FreeType library");
        s_FreeType = msdfgen::initializeFreetype();

        if (!s_FreeType) {
            MC_ERROR("Failed to initialize free type font handle!");
            return nullptr;
        }
    }

    // Load font  TODO: HANDLE not being in first chunk
    auto ttfData = Motes::ReadCompressedBinaryData(data.Chunks[0]);

    // Charset type
    FontCharset charset =
        Motes::ChunkToStruct<FontMetadata>(data.Chunks[1])->Charset;

    return CreateRef<Font>(charset, ttfData);
}

Font::Font(const FontCharset charsetSrc, const std::vector<uint8_t> &ttfData) {
    msdfgen::FontHandle *font =
        msdfgen::loadFontData(s_FreeType, ttfData.data(), ttfData.size());
    if (!font) {
        MC_ERROR("Failed to load font from TTF data!");
        return;
    }

    if (charsetSrc != ASCII)
        M_WARN("Unkown charset type! Only ASCII is supported");

    // Charset
    const msdf_atlas::Charset &charset = msdf_atlas::Charset::ASCII;
    MC_TRACE("Charset: ASCII");

    // Storage for map of glyph geometry, and load from font
    m_Geometry = msdf_atlas::FontGeometry(&m_Glyphs);
    int glyphsLoaded = m_Geometry.loadCharset(font, 1.0, charset);
    MC_TRACE("Loaded {} glyphs out of {}", glyphsLoaded, charset.size());

    // Apply MSDF edge coloring. See edge-coloring.h for other coloring
    // strategies.
    constexpr double angleThreshold = 3.0;
    for (msdf_atlas::GlyphGeometry &glyph : m_Glyphs) {
        coloringSeed *= lcgMultiplier;
        // TODO: PICK BETWEEN INKTRAP AND SIMPLE
        glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, angleThreshold,
                           coloringSeed);
    }

    // Pack characters and create atlas
    glm::ivec2 size = PackCharactersTightly(m_Glyphs);
    ImageData imageData = CreateAtlas(size, m_Glyphs);
    MC_TRACE("(Bitmap W: {}, H: {}, C: {})", imageData.Width, imageData.Height,
             imageData.Channels);
    m_Texture =
        Application::Instance()->GetGraphicsAPI().CreateTexture(&imageData);

    // We don't neeed the font anymore
    msdfgen::destroyFont(font);

    // Calculate metrics etc.
    float texelWidth = 1.0f / imageData.Width;
    float texelHeight = 1.0f / imageData.Height;
    m_TexelScale = glm::vec4(texelWidth, texelHeight, texelWidth, texelHeight);
    m_TabOffset = Glyph(' ')->getAdvance() * 4;
}

const msdf_atlas::GlyphGeometry *
Font::Glyph(const msdf_atlas::unicode_t c) const {
    return m_Geometry.getGlyph(c);
}

glm::vec4 Font::AtlasBounds(const msdf_atlas::GlyphGeometry *glyph) const {
    double al, ab, ar, at;
    glyph->getQuadAtlasBounds(al, ab, ar, at);
    return glm::vec4((float)al, (float)ab, (float)ar, (float)at) * m_TexelScale;
}

glm::vec4 Font::PosBounds(const msdf_atlas::GlyphGeometry *glyph) const {
    double l, b, r, t;
    glyph->getQuadPlaneBounds(l, b, r, t);
    return glm::vec4((float)l, (float)b, (float)r, (float)t);
}
} // namespace Mana
