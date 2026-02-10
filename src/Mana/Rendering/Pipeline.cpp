#include "Pipeline.h"

#include "Mana/Core/Application.h"
#include "Mana/Rendering/Passes/ClearFramebufferPass.h"
#include "Mana/Rendering/Passes/PPOutline.h"
#include "Mana/Rendering/Passes/ScenePass.h"
#include "Mana/Rendering/Passes/UIPass.h"

namespace Mana {
RenderPass::RenderPass(GraphicsContext &c) { API = c.GetAPI(); }
GraphicsPipeline::GraphicsPipeline(GraphicsContext &context) {
    m_API = context.GetAPI();

    m_Renderpasses.push_back(CreateScope<ClearFramebufferPass>(
        context, Application::Instance()->GetConfig().Rendering_ClearColor));
    m_Renderpasses.push_back(CreateScope<ScenePass>(context));
    m_Renderpasses.push_back(CreateScope<PPOutline>(context));
    m_Renderpasses.push_back(CreateScope<UIPass>(context));

    // Resize to the correct size
    auto logicalSize = Application::Instance()->GetWindow().GetWindowSize();
    auto framebufferSize =
        Application::Instance()->GetWindow().GetFramebufferSize();
    m_FrameData.ScreenCamera = CreateScope<OrthographicCamera>(
        0.0f, static_cast<float>(logicalSize.x), 0.0f,
        static_cast<float>(logicalSize.y));
    m_API->SetViewport(0, 0, framebufferSize.x, framebufferSize.y);
}

void GraphicsPipeline::OnEvent(Event *e) {
    EventDispatcher dispatcher(e);

    dispatcher.Dispatch<WindowResizeEvent>(M_BIND_EVENT_FN(OnWindowResize));
    dispatcher.Dispatch<WindowFramebufferResizeEvent>(
        M_BIND_EVENT_FN(OnFramebufferResize));
}

bool GraphicsPipeline::OnWindowResize(WindowResizeEvent &e) {
    MC_TRACE("Resizing UI/Screen camera projection");
    m_FrameData.ScreenCamera->SetProjection(
        0.0f, static_cast<float>(e.GetWidth()), 0.0f,
        static_cast<float>(e.GetHeight()));

    for (auto &pass : m_Renderpasses) {
        pass->OnWindowResize(e);
    }

    return false;
}

bool GraphicsPipeline::OnFramebufferResize(WindowFramebufferResizeEvent &e) {
    MC_TRACE("Updating viewport size");
    m_API->SetViewport(0, 0, e.GetWidth(), e.GetHeight());

    for (auto &pass : m_Renderpasses) {
        pass->OnFramebufferResize(e);
    }

    return false;
}

void GraphicsPipeline::RenderFrame() {
    for (auto &pass : m_Renderpasses) {
        pass->Execute(&m_FrameData);
    }

    m_FrameData.Clear();

    m_API->SwapWindowDrawBuffers();
}

} // namespace Mana
