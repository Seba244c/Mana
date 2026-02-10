#include "ScenePass.h"

namespace Mana {
ScenePass::ScenePass(GraphicsContext &context)
    : RenderPass(context), m_Renderer(context) {}

void ScenePass::OnWindowResize(const WindowResizeEvent &e) {}
void ScenePass::OnFramebufferResize(const WindowFramebufferResizeEvent &e) {}
void ScenePass::Execute(FrameData *data) {
    m_Renderer.Open(data->SceneCamera);
    DrawTilemaps(data);
    DrawSprites(data);
    m_Renderer.Close();
}

void ScenePass::DrawTilemaps(FrameData *data) {
    for (const auto &chunk : data->TilemapChunks) {
        uint8_t spriteId = 0;

        for (int iy = 0; iy < 8; iy++) {
            glm::vec2 curr = {chunk.Origin.x,
                              chunk.Origin.y + chunk.SpriteSize * iy};
            for (int ix = 0; ix < 8; ix++) {
                spriteId = chunk.Chunk.Grid[ix][iy];
                if (spriteId != 0) {
                    m_Renderer.DrawQuad(
                        {curr,                                // bl
                         {curr.x + chunk.SpriteSize, curr.y}, // br
                         {curr.x + chunk.SpriteSize,
                          curr.y + chunk.SpriteSize},         // tr
                         {curr.x, curr.y + chunk.SpriteSize}, // tl
                         chunk.Sheet->GetTexture(),
                         Colors::White,
                         chunk.Sheet->TexCoords(spriteId)});
                }
                curr.x += chunk.SpriteSize;
            }
        }
    }
}
void ScenePass::DrawSprites(FrameData *data) {
    std::sort(data->Sprites.begin(), data->Sprites.end(),
              [](const DrwSprite &a, const DrwSprite &b) {
                  return a.Layer < b.Layer;
              });
    for (const auto &sprite : data->Sprites) {
        m_Renderer.DrawQuad({sprite.Center, sprite.Size, sprite.Texture,
                             sprite.Color, sprite.RotationZ});
    }
}
} // namespace Mana
