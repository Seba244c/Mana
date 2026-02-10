#pragma once
#include "mpch.h"
#include <ext/import-font.h>

#include "Mana/Core/Asset.h"
#include "Mana/Graphics/Texture.h"

#include "msdf-atlas-gen/FontGeometry.h"
#include "msdf-atlas-gen/GlyphGeometry.h"
#include "msdf-atlas-gen/types.h"

namespace Mana {
enum FontCharset : uint8_t {
    ASCII,
};

enum FontSourceType : uint8_t {
    TTF,
};

struct FontMetadata {
    FontCharset Charset;
    FontSourceType SourceType;
};

class Font {
  public:
    static void CreateMote(std::filesystem::path src,
                           std::vector<MoteChunkOwned> &data);
    static Ref<Font> Create(const MoteData &data);

  public:
    Font(const FontCharset charset, const std::vector<uint8_t> &ttfData);

    const Ref<Texture> &GetTexture() const { return m_Texture; }

    const msdf_atlas::GlyphGeometry *Glyph(const msdf_atlas::unicode_t c) const;
    glm::vec4 AtlasBounds(const msdf_atlas::GlyphGeometry *glyph) const;
    glm::vec4 PosBounds(const msdf_atlas::GlyphGeometry *glyph) const;
    const float GetAdvance(double &advance, const msdf_atlas::unicode_t c1,
                           const msdf_atlas::unicode_t c2) const {
        return m_Geometry.getAdvance(advance, c1, c2);
    }
    const msdfgen::FontMetrics &Metrics() const {
        return m_Geometry.getMetrics();
    }
    const float TabOffset() const { return m_TabOffset; }

  private:
    Ref<Texture> m_Texture;
    std::vector<msdf_atlas::GlyphGeometry> m_Glyphs;
    msdf_atlas::FontGeometry m_Geometry;
    glm::vec4 m_TexelScale;

    float m_TabOffset;
};
} // namespace Mana
