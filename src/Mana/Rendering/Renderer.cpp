#include "Renderer.h"

#include "Mana/Core/Application.h"
#include "Mana/Core/Asset.h"
#include "Mana/Core/Log.h"
#include "Mana/Graphics/GraphicsContext.h"
#include "Mana/Graphics/Texture.h"
#include "Mana/Rendering/Camera.h"
#include "Mana/Rendering/Font.h"
#include "Mana/World/ECS.h"

namespace Mana {
static glm::vec2 corners[4] = {
    {-0.5f, -0.5f}, {0.5f, -0.5f}, {0.5f, 0.5f}, {-0.5f, 0.5f}};

std::array<glm::vec2, 4> Renderer2D::Quad::GenCorners(const glm::vec2 center,
                                                      const glm::vec2 size,
                                                      const float rotationZ) {
    glm::mat2 rotMatrix = {{cos(rotationZ), -sin(rotationZ)},
                           {sin(rotationZ), cos(rotationZ)}};

    std::array<glm::vec2, 4> rotatedCorners;
    for (int i = 0; i < 4; ++i) {
        glm::vec2 scaledCorner = size * corners[i];         // Scale the corner
        glm::vec2 rotatedCorner = rotMatrix * scaledCorner; // Rotate the corner
        rotatedCorners[i] = center + rotatedCorner; // Translate back to center
    }

    return rotatedCorners;
}

Renderer2D::Quad::Quad(const glm::vec2 center, const glm::vec2 size,
                       const Mana::Texture *texture, const Mana::Color color,
                       const float rotationZ)
    : Corners(GenCorners(center, size, rotationZ)), Texture(texture),
      Color(color), TexCoords(DefaultTextcoords) {}

Renderer2D::Renderer2D(GraphicsContext &context) : m_API(context.GetAPI()) {
    // Create Projection Mtrix
    auto fboSize = Application::Instance()->GetWindow().GetFramebufferSize();

    // Create what's needed for **Quad Rendering**
    m_Quads.Shader = M_ASSET(Shader, "shaders/Renderer2D_Quad.glsl");
    m_Quads.VertexArray = m_API->CreateVertexArray();
    m_Quads.VertexBuffer =
        m_API->CreateVertexBuffer(m_Quads.MaxVertices * sizeof(QuadVertex));
    m_Quads.VertexBuffer->SetLayout({
        {ShaderDataType::Float2, "a_Position"},
        {ShaderDataType::Float4, "a_Color"},
        {ShaderDataType::UInt, "a_TexIndex"},
        {ShaderDataType::Float2, "a_TexCoord"},
    });

    m_Quads.VertexArray->AddVertexBuffer(m_Quads.VertexBuffer);
    m_Quads.VertexBufferBase = new QuadVertex[m_Quads.MaxVertices];
    m_Quads.VertexBufferPtr = m_Quads.VertexBufferBase;
    m_Quads.IndexCount = 0;

    // Since quad indices are equal for all quads (2 triangles) we populate the
    // index buffer once
    auto quadIndices = new uint32_t[m_Quads.MaxIndices];
    uint32_t offset = 0; // Current indcy

    for (uint32_t i = 0; i < m_Quads.MaxIndices; i += 6) {
        quadIndices[i + 0] = offset + 0;
        quadIndices[i + 1] = offset + 1;
        quadIndices[i + 2] = offset + 2;

        quadIndices[i + 3] = offset + 2;
        quadIndices[i + 4] = offset + 3;
        quadIndices[i + 5] = offset + 0;

        offset += 4;
    }

    Ref<IndexBuffer> quadIB =
        m_API->CreateIndexBuffer(quadIndices, m_Quads.MaxIndices);
    m_Quads.VertexArray->SetIndexBuffer(quadIB);
    delete[] quadIndices;

    // We center the quads around their Position, by moving thet vertexes 50%
    // out form the center
    m_Quads.VertexPositions[0] = {-0.5f, -0.5f, 0.0f, 1.0f};
    m_Quads.VertexPositions[1] = {0.5f, -0.5f, 0.0f, 1.0f};
    m_Quads.VertexPositions[2] = {0.5f, 0.5f, 0.0f, 1.0f};
    m_Quads.VertexPositions[3] = {-0.5f, 0.5f, 0.0f, 1.0f};

    // Everything we need to render lines
    m_Lines.Shader = M_ASSET(Shader, "shaders/Renderer2D_Line.glsl");
    m_Lines.VertexArray = m_API->CreateVertexArray();
    m_Lines.VertexBuffer =
        m_API->CreateVertexBuffer(m_Lines.MaxVertices * sizeof(LineVertex));
    m_Lines.VertexBuffer->SetLayout({{ShaderDataType::Float2, "a_Position"},
                                     {ShaderDataType::Float4, "a_Color"}});

    m_Lines.VertexArray->AddVertexBuffer(m_Lines.VertexBuffer);
    m_Lines.VertexBufferBase = new LineVertex[m_Lines.MaxVertices];
    m_Lines.VertexBufferPtr = m_Lines.VertexBufferBase;
    m_Lines.IndexCount = 0;
    // Since line indices are equal for all lines we populate the
    // index buffer once
    auto lineIndices = new uint32_t[m_Lines.MaxIndices];
    offset = 0; // Current indcy

    for (uint32_t i = 0; i < m_Lines.MaxIndices; i += 2) {
        lineIndices[i + 0] = offset + 0;
        lineIndices[i + 1] = offset + 1;

        offset += 2;
    }

    Ref<IndexBuffer> lineIB =
        m_API->CreateIndexBuffer(lineIndices, m_Lines.MaxIndices);
    m_Lines.VertexArray->SetIndexBuffer(lineIB);
    delete[] lineIndices;

    // Creat the fully white texture
    uint32_t color = 0xFFFFFF; // White pixel
    std::vector<uint8_t> pixelData = {
        static_cast<uint8_t>((color >> 16) & 0xFF), // Red
        static_cast<uint8_t>((color >> 8) & 0xFF),  // Green
        static_cast<uint8_t>(color & 0xFF)          // Blue
    };

    ImageData TextureData{1, 1, 3, pixelData};
    m_WhiteTexture = m_API->CreateTexture(&TextureData);

// Prepare TextureSlots
#if MANAB_OPENGL_MAX_TEXTURE < 2
    m_TextureSlots = new const Texture *[1];
#else
    m_TextureSlots = new const Texture *[m_API->GetMaxFragmentSamplers()];
    m_TextureSlots[0] = m_WhiteTexture.get();
#endif

    // Prepare for text
    m_Text.Shader = M_ASSET(Shader, "shaders/Renderer2D_Text.glsl");
    m_Text.VertexArray = m_API->CreateVertexArray();
    m_Text.VertexBuffer =
        m_API->CreateVertexBuffer(m_Text.MaxVertices * sizeof(TextVertex));
    m_Text.VertexBuffer->SetLayout({
        {ShaderDataType::Float2, "a_Position"},
        {ShaderDataType::Float4, "a_FgColor"},
        {ShaderDataType::UInt, "a_TexIndex"},
        {ShaderDataType::Float2, "a_TexCoord"},
    });

    m_Text.VertexArray->AddVertexBuffer(m_Text.VertexBuffer);
    m_Text.VertexBufferBase = new TextVertex[m_Text.MaxVertices];
    m_Text.VertexBufferPtr = m_Text.VertexBufferBase;
    m_Text.IndexCount = 0;

    auto textIndices = new uint32_t[m_Text.MaxIndices];
    offset = 0; // Current indcy

    for (uint32_t i = 0; i < m_Text.MaxIndices; i += 6) {
        textIndices[i + 0] = offset + 0;
        textIndices[i + 1] = offset + 1;
        textIndices[i + 2] = offset + 2;

        textIndices[i + 3] = offset + 2;
        textIndices[i + 4] = offset + 3;
        textIndices[i + 5] = offset + 0;

        offset += 4;
    }

    Ref<IndexBuffer> textIB =
        m_API->CreateIndexBuffer(textIndices, m_Text.MaxIndices);
    m_Text.VertexArray->SetIndexBuffer(textIB);
    delete[] textIndices;
}

Renderer2D::~Renderer2D() {
    delete[] m_Quads.VertexBufferBase;
    delete[] m_Lines.VertexBufferBase;
    delete[] m_Text.VertexBufferBase;
    delete[] m_TextureSlots;
}

void Renderer2D::Open(const Camera *camera) {
    Flush();
    m_Camera = camera;
}

void Renderer2D::Close() { Flush(); }

void Renderer2D::RenderQuads(const Ref<Shader> &shader) {
    // Upload Vertex data
    uint32_t dataSize = (uint32_t)((uint8_t *)m_Quads.VertexBufferPtr -
                                   (uint8_t *)m_Quads.VertexBufferBase);
    m_Quads.VertexBuffer->SetData(m_Quads.VertexBufferBase, dataSize);

    // Prepare to draw
    shader->Bind();
    shader->SetMatrix("u_ViewProjMatrix", m_Camera->GetViewProjectionMatrix());

    // Bind textures
    for (uint8_t i = 0; i < m_NextTextureSlot; i++)
        m_TextureSlots[i]->Bind(i);

    m_API->DrawIndexed(m_Quads.VertexArray, m_Quads.IndexCount);

    // New Batch
    m_Quads.IndexCount = 0;
    m_Quads.VertexBufferPtr = m_Quads.VertexBufferBase;
}

void Renderer2D::RenderText(const Ref<Shader> &shader) {
    // Upload Vertex data
    uint32_t dataSize = (uint32_t)((uint8_t *)m_Text.VertexBufferPtr -
                                   (uint8_t *)m_Text.VertexBufferBase);
    m_Text.VertexBuffer->SetData(m_Text.VertexBufferBase, dataSize);

    // Prepare to draw
    shader->Bind();
    shader->SetMatrix("u_ViewProjMatrix", m_Camera->GetViewProjectionMatrix());

    // Bind textures
    for (uint8_t i = 0; i < m_NextTextureSlot; i++)
        m_TextureSlots[i]->Bind(i);

    m_API->DrawIndexed(m_Text.VertexArray, m_Text.IndexCount);

    // New Batch
    m_Text.IndexCount = 0;
    m_Text.VertexBufferPtr = m_Text.VertexBufferBase;
}

void Renderer2D::RenderLines(const Ref<Shader> &shader) {
    // Upload Vertex data
    uint32_t dataSize = (uint32_t)((uint8_t *)m_Lines.VertexBufferPtr -
                                   (uint8_t *)m_Lines.VertexBufferBase);
    m_Lines.VertexBuffer->SetData(m_Lines.VertexBufferBase, dataSize);

    // Prepare to draw
    shader->Bind();
    shader->SetMatrix("u_ViewProjMatrix", m_Camera->GetViewProjectionMatrix());

    m_API->DrawIndexedLines(m_Lines.VertexArray, m_Lines.IndexCount);

    // New Batch
    m_Lines.IndexCount = 0;
    m_Lines.VertexBufferPtr = m_Lines.VertexBufferBase;
}

void Renderer2D::Flush() {
    if (m_Quads.IndexCount)
        RenderQuads(m_Quads.Shader);

    if (m_Text.IndexCount)
        RenderText(m_Text.Shader);

    if (m_Lines.IndexCount)
        RenderLines(m_Lines.Shader);

    // Reset TextureSlots
#if MANAB_OPENGL_MAX_TEXTURE < 2
    m_TextureSlots[0] = nullptr;
#else
    m_NextTextureSlot = 1;
    for (size_t i = 1; i < m_API->GetMaxFragmentSamplers(); ++i) {
        m_TextureSlots[i] = nullptr;
    }
#endif
}

#if MANAB_OPENGL_MAX_TEXTURE > 1
uint8_t Renderer2D::GetTextureIndex(const Texture *texture) {
    for (int i = 1; i < m_API->GetMaxFragmentSamplers(); i++) {
        if (m_TextureSlots[i] == texture) {
            return i;
        }
    }

    if (m_NextTextureSlot >= m_API->GetMaxFragmentSamplers())
        Flush();

    m_TextureSlots[m_NextTextureSlot] = texture;
    return m_NextTextureSlot++;
}
#endif

std::array<glm::vec2, 4> Renderer2D::Quad::DefaultTextcoords = {
    glm::vec2{0.0f, 0.0f}, // Bottom-left
    {1.0f, 0.0f},          // Bottom-right
    {1.0f, 1.0f},          // Top-right
    {0.0f, 1.0f}           // Top-left
};

void Renderer2D::DrawQuad(const Quad &q) {
    if (m_Quads.IndexCount >= m_Quads.MaxIndices)
        Flush();

#if MANAB_OPENGL_MAX_TEXTURE < 2
    uint32_t texIndex = 0;
    const Texture *tex = q.Texture ? q.Texture : m_WhiteTexture.get();
    if (m_TextureSlots[0] != tex) {
        Flush();
        m_TextureSlots[0] = tex;
    }
#else
    // We have to get the texture index
    uint32_t texIndex = q.Texture ? GetTextureIndex(q.Texture) : 0;
#endif

    constexpr size_t quadVertexCount = 4;

    // Add the 4 vertices
    for (size_t i = 0; i < quadVertexCount; i++) {
        m_Quads.VertexBufferPtr->Position = q.Corners[i];
        m_Quads.VertexBufferPtr->Color = q.Color;
        m_Quads.VertexBufferPtr->TexIndex = texIndex;
        m_Quads.VertexBufferPtr->TexCoords = (q.TexCoords)[i];
        m_Quads.VertexBufferPtr++;
    }

    // Increment the index count by 2*3 (for 2 * triangles)
    m_Quads.IndexCount += 6;
}

static Ref<Texture> nullTexture = nullptr;
Renderer2D::Quad LineToQuad(const Renderer2D::Line &line) {
    glm::vec2 a2D = glm::vec2(line.PointA);
    glm::vec2 b2D = glm::vec2(line.PointB);

    glm::vec2 direction = glm::normalize(b2D - a2D);
    glm::vec2 normal = glm::vec2(-direction.y, direction.x); // perpendicular

    float halfWidth = line.Width * 0.5f;
    glm::vec2 offset = normal * halfWidth;

    glm::vec2 p1 = a2D + offset; // top-left
    glm::vec2 p2 = a2D - offset; // bottom-left
    glm::vec2 p3 = b2D - offset; // bottom-right
    glm::vec2 p4 = b2D + offset; // top-right

    return {p1, p2, p3, p4, nullTexture, line.Color};
}

void Renderer2D::DrawLine(const Line &l) {
    if (l.Width != 1) {
        DrawQuad(LineToQuad(l));
        return;
    }

    if (m_Lines.IndexCount >= m_Lines.MaxIndices)
        Flush();

    constexpr size_t lineVertexCount = 2;

    // Add the 2 vertices
    m_Lines.VertexBufferPtr->Position = l.PointA;
    m_Lines.VertexBufferPtr->Color = l.Color;
    m_Lines.VertexBufferPtr++;
    m_Lines.VertexBufferPtr->Position = l.PointB;
    m_Lines.VertexBufferPtr->Color = l.Color;
    m_Lines.VertexBufferPtr++;

    m_Lines.IndexCount += 2;
}

void Renderer2D::DrawLabel(const Label &l) {
// Get a texture slot
#if MANAB_OPENGL_MAX_TEXTURE < 2
    uint32_t texIndex = 0;
    if (m_TextureSlots[0] != l.Font->GetTexture().get()) {
        Flush();
        m_TextureSlots[0] = l.Font->GetTexture().get();
    }
#else
    uint32_t texIndex = GetTextureIndex(l.Font->GetTexture().get());
#endif

    const auto &metrics = l.Font->Metrics();
    double fsScale = l.Size / metrics.lineHeight;
    float xOffset = 0;
    float yOffset = -metrics.descenderY * fsScale;

    // For each character
    for (size_t i = 0; i < l.String.size(); i++) {
        if (m_Text.IndexCount >= m_Text.MaxIndices)
            Flush();

        char character = l.String[i];
        auto glyph = l.Font->Glyph(character);
        M_ASSERT(glyph);

        switch (character) { // Handle spacing characters
        case '\r':
            continue;
        case '\n':
            MC_ERROR("Newlines not supported");
            continue;
        case '\t':
            xOffset = (std::floor(xOffset / l.Font->TabOffset()) + 1) *
                      l.Font->TabOffset();
            continue;
        case ' ':
            double advance;
            if (i < l.String.size() - 1) { // Advance by character width
                char nextCharacter = l.String[i + 1];
                l.Font->GetAdvance(advance, character, nextCharacter);

                xOffset += fsScale * advance;
            }
            continue;
        }

        // Get the glyph data
        auto texCoords = l.Font->AtlasBounds(glyph);
        auto planeBounds = l.Font->PosBounds(glyph);
        planeBounds *= fsScale;
        planeBounds += glm::vec4(xOffset + l.Bl.x, yOffset + l.Bl.y,
                                 xOffset + l.Bl.x, yOffset + l.Bl.y);

        // Add the 4 vertices
        Color fgColor = l.Color;
        constexpr size_t charVertexCount = 4;
        m_Text.VertexBufferPtr->Position = {planeBounds.x, planeBounds.y};
        m_Text.VertexBufferPtr->FgColor = fgColor;
        m_Text.VertexBufferPtr->TexIndex = texIndex;
        m_Text.VertexBufferPtr->TexCoords = {texCoords.x, texCoords.y};
        m_Text.VertexBufferPtr++;
        m_Text.VertexBufferPtr->Position = {planeBounds.z, planeBounds.y};
        m_Text.VertexBufferPtr->FgColor = fgColor;
        m_Text.VertexBufferPtr->TexIndex = texIndex;
        m_Text.VertexBufferPtr->TexCoords = {texCoords.z, texCoords.y};
        m_Text.VertexBufferPtr++;
        m_Text.VertexBufferPtr->Position = {planeBounds.z, planeBounds.w};
        m_Text.VertexBufferPtr->FgColor = fgColor;
        m_Text.VertexBufferPtr->TexIndex = texIndex;
        m_Text.VertexBufferPtr->TexCoords = {texCoords.z, texCoords.w};
        m_Text.VertexBufferPtr++;
        m_Text.VertexBufferPtr->Position = {planeBounds.x, planeBounds.w};
        m_Text.VertexBufferPtr->FgColor = fgColor;
        m_Text.VertexBufferPtr->TexIndex = texIndex;
        m_Text.VertexBufferPtr->TexCoords = {texCoords.x, texCoords.w};
        m_Text.VertexBufferPtr++;

        // Increment the index count by 2*3 (for 2 * triangles)
        m_Text.IndexCount += 6;

        if (i < l.String.size() - 1) { // Advance by character width
            double advance;
            char nextCharacter = l.String[i + 1];
            l.Font->GetAdvance(advance, character, nextCharacter);

            xOffset += fsScale * advance;
        }
    }
}

void Renderer2D::LabelWidth(const std::string_view s, const Font *f,
                            const float size, float &out) {
    out = 0;
    const auto &metrics = f->Metrics();
    double fsScale = size / metrics.lineHeight;
    for (size_t i = 0; i < s.size(); i++) {
        char character = s[i];
        auto glyph = f->Glyph(character);
        M_ASSERT(glyph);

        switch (character) { // Handle spacing characters
        case '\r':
            continue;
        case '\n':
            MC_ERROR("Newlines not supported");
            continue;
        case '\t':
            out = (std::floor(out / f->TabOffset()) + 1) * f->TabOffset();
            continue;
        default:
            double advance;
            advance = glyph->getAdvance();
            if (i < s.size() - 1) { // Advance by character width
                char nextCharacter = s[i + 1];
                f->GetAdvance(advance, character, nextCharacter);
            }
            out += fsScale * advance;
            continue;
        }
    }
}

void Renderer2D::Draw(const DrawData &data) {
    for (const Quad &q : data.Quads) {
        DrawQuad(q);
    }

    for (const Line &l : data.Lines) {
        DrawLine(l);
    }

    for (const Label &l : data.Labels) {
        DrawLabel(l);
    }
}

Renderer2DModule::Renderer2DModule(flecs::world &world) {
    MC_TRACE("Registering ECS Module: Renderer2D");
    // Import Camera Module
    world.import <CameraModule>();

    // Register Rendering System
    world.system<CameraController>("Find ActiveCamera")
        .kind(ECS::Phases.OnRender)
        .run([](flecs::iter &it) {
            flecs::entity activeCameraEntity;
            flecs::entity last;
            bool foundActiveCamera = false;

            while (it.next()) {
                for (auto i : it) {
                    flecs::entity e = it.entity(i); // Get the entity at index i
                    last = e;

                    if (e.has<CameraModule::ActiveCamera>()) {
                        foundActiveCamera = true;
                        activeCameraEntity = e;
                        break;
                    }
                }
            }
            M_ASSERT(last, "NO CAMERAS IN WORLD");

            // If no active camera was found, add ActiveCamera to the first
            // CameraController
            if (!foundActiveCamera) {
                MC_WARN("Found no active camera, adding ActiveCamera to the "
                        "last camera");
                last.add<CameraModule::ActiveCamera>(); // Add ActiveCamera
                                                        // to the first
                                                        // CameraController
                activeCameraEntity = last;
            }

            if (auto cam = activeCameraEntity.try_get_mut<OrthoCamera>())
                Application::Instance()
                    ->GetGraphicsPipeline()
                    .FrameData()
                    .SceneCamera = cam->GetCamera();
            else if (auto cam =
                         activeCameraEntity.try_get_mut<CameraController>())
                Application::Instance()
                    ->GetGraphicsPipeline()
                    .FrameData()
                    .SceneCamera = cam->GetCamera();
        });
}
} // namespace Mana
