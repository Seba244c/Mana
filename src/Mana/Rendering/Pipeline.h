#pragma once
#include <mpch.h>

#include "Mana/Events/IOEvents.h"
#include "Mana/Graphics/GraphicsContext.h"
#include "Mana/Rendering/Camera.h"
#include "Mana/Rendering/Cameras/Orthographic.h"
#include "Mana/Rendering/Rendercommand.h"

namespace Mana {
struct FrameData {
    const Camera *SceneCamera;
    Scope<OrthographicCamera> ScreenCamera;
    std::vector<DrwSprite> Sprites;
    std::vector<DrwSprite> PPSpriteOutline;
    std::vector<DrwTilemapChunk> TilemapChunks;

    void Clear() {
        SceneCamera = nullptr;
        Sprites.clear();
        PPSpriteOutline.clear();
        TilemapChunks.clear();
    }
};

class RenderPass {
  public:
    explicit RenderPass(GraphicsContext &context);
    virtual ~RenderPass() = default;

    virtual void OnWindowResize(const WindowResizeEvent &e) = 0;
    virtual void OnFramebufferResize(const WindowFramebufferResizeEvent &e) = 0;
    virtual void Execute(FrameData *data) = 0;

  protected:
    Scope<GraphicsAPI> API;
};

class GraphicsPipeline {
  public:
    GraphicsPipeline(GraphicsContext &context);

    void OnEvent(Event *e);
    void RenderFrame();

    FrameData &FrameData() { return m_FrameData; }

  private:
    bool OnWindowResize(WindowResizeEvent &e);
    bool OnFramebufferResize(WindowFramebufferResizeEvent &e);

  private:
    struct FrameData m_FrameData;
    std::vector<Scope<RenderPass>> m_Renderpasses;

    Scope<GraphicsAPI> m_API;
};

} // namespace Mana
