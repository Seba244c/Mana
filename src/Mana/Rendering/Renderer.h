#pragma once

#include <mpch.h>

#include "Mana/Graphics/GraphicsAPI.h"
#include "Mana/Graphics/GraphicsContext.h"
#include "Mana/Graphics/Shader.h"
#include "Mana/Graphics/Texture.h"
#include "Mana/Rendering/Camera.h"
#include "Mana/Rendering/Font.h"

namespace Mana {
class Renderer2D {
  public:
    struct Quad {
        static std::array<glm::vec2, 4> DefaultTextcoords;
        Quad(const glm::vec2 center = {0.0f, 0.0f},
             const glm::vec2 size = {1.0f, 1.0f},
             const Texture *texture = nullptr,
             const Color color = Colors::White, const float rotationZ = 0.0f);

        Quad(const glm::vec2 bottomLeft, const glm::vec2 bottomRight,
             const glm::vec2 topRight, const glm::vec2 topLeft,
             const Ref<Texture> &texture = nullptr,
             const Color color = Colors::White,
             const std::array<glm::vec2, 4> TexCoords = DefaultTextcoords)
            : Corners({bottomLeft, bottomRight, topRight, topLeft}),
              Texture(texture.get()), Color(color), TexCoords(TexCoords) {}

        Quad(const glm::vec2 bottomLeft, const glm::vec2 bottomRight,
             const glm::vec2 topRight, const glm::vec2 topLeft,
             const Texture *texture = nullptr,
             const Color color = Colors::White,
             const std::array<glm::vec2, 4> TexCoords = DefaultTextcoords)
            : Corners({bottomLeft, bottomRight, topRight, topLeft}),
              Texture(texture), Color(color), TexCoords(TexCoords) {}

        static std::array<glm::vec2, 4> GenCorners(const glm::vec2 center,
                                                   const glm::vec2 size,
                                                   const float rotationZ);

        std::array<glm::vec2, 4> Corners;
        const Texture *Texture;
        Color Color;
        std::array<glm::vec2, 4> TexCoords;
    };

    struct Line {
        glm::vec2 PointA = {0.0f, 0.0f};
        glm::vec2 PointB = {0.0f, 0.0f};
        Color Color = Colors::White;
        float Width = 1;
    };

    struct Label {
        glm::vec2 Bl;
        std::string String;
        const Font *Font;
        float Size;
        Color Color;
    };

    struct DrawData {
        std::vector<Quad> Quads;
        std::vector<Line> Lines;
        std::vector<Label> Labels;

        void Clear() {
            Quads.clear();
            Lines.clear();
            Labels.clear();
        }
    };

    struct QuadVertex {
        glm::vec2 Position;
        glm::vec4 Color;
        uint32_t TexIndex;
        glm::vec2 TexCoords;
    };

    struct LineVertex {
        glm::vec2 Position;
        glm::vec4 Color;
    };

    struct TextVertex {
        glm::vec2 Position;
        glm::vec4 FgColor;
        uint32_t TexIndex;
        glm::vec2 TexCoords;
    };

    struct QuadData {
        static constexpr uint32_t MaxQuads = 20000;
        static constexpr uint32_t MaxVertices = MaxQuads * 4;
        static constexpr uint32_t MaxIndices = MaxQuads * 6;

        // Quads
        Ref<Shader> Shader;
        Ref<VertexArray> VertexArray;
        uint32_t IndexCount = 0;
        Ref<VertexBuffer> VertexBuffer;
        QuadVertex *VertexBufferBase = nullptr;
        QuadVertex *VertexBufferPtr = nullptr;
        glm::vec4 VertexPositions[4];
    };

    struct LineData {
        static constexpr uint32_t MaxLines = 1000;
        static constexpr uint32_t MaxVertices = MaxLines * 2;
        static constexpr uint32_t MaxIndices = MaxLines * 2;

        Ref<Shader> Shader;
        Ref<VertexArray> VertexArray;
        uint32_t IndexCount = 0;
        Ref<VertexBuffer> VertexBuffer;
        LineVertex *VertexBufferBase = nullptr;
        LineVertex *VertexBufferPtr = nullptr;
        float CurrentWidth = 1;
    };

    struct TextData {
        static constexpr uint32_t MaxChars = 1000;
        static constexpr uint32_t MaxVertices = MaxChars * 4;
        static constexpr uint32_t MaxIndices = MaxChars * 6;
        Ref<Shader> Shader;
        Ref<VertexArray> VertexArray;
        uint32_t IndexCount = 0;
        Ref<VertexBuffer> VertexBuffer;
        TextVertex *VertexBufferBase = nullptr;
        TextVertex *VertexBufferPtr = nullptr;
        glm::vec4 VertexPositions[4];
    };

  public:
    Renderer2D(GraphicsContext &context);
    ~Renderer2D();

    void SetQuadShader(Ref<Shader> shader) { m_Quads.Shader = shader; }
    void DrawQuad(const Quad &quad);
    void DrawLine(const Line &line);
    void DrawLabel(const Label &label);
    void Draw(const DrawData &data);
    void Open(const Camera *camera);
    void Close();
    static void LabelWidth(const std::string_view s, const Font *f,
                           const float size, float &out);

  protected:
    void Flush();
    void RenderLines(const Ref<Shader> &shader);
    void RenderText(const Ref<Shader> &shader);
    void RenderQuads(const Ref<Shader> &shader);
    uint8_t GetTextureIndex(const Texture *texture);

  private:
    Scope<GraphicsAPI> m_API;
    QuadData m_Quads;
    LineData m_Lines;
    TextData m_Text;
    const Camera *m_Camera;

    Ref<Texture> m_WhiteTexture;

    uint32_t m_NextTextureSlot = 1; // 0 = white texture
    const Texture **m_TextureSlots;
};

class Renderer2DModule {
  public:
    Renderer2DModule(flecs::world &world);
};
} // namespace Mana
