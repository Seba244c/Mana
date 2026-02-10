#include "UIPass.h"

#include "Mana/Core/Application.h"
#include "Mana/Rendering/UI.h"

namespace Mana {
UIPass::UIPass(GraphicsContext &context)
    : RenderPass(context), m_Renderer(context) {}

void UIPass::OnWindowResize(const WindowResizeEvent &e) {
    MC_TRACE("Resizing existing UI layers");

    // The layerstack should be dimensioned in the logical window size
    for (Layer *layer : Application::Instance()->GetLayerstack()) {
        layer->Size({e.GetWidth(), e.GetHeight()});
    }
}
void UIPass::OnFramebufferResize(const WindowFramebufferResizeEvent &e) {}

void UIPass::Execute(FrameData *data) {
    m_Renderer.Open(&*data->ScreenCamera);
    UI::Clear();
    for (Layer *layer : Application::Instance()->GetLayerstack())
        UI::Draw(layer);

    m_Renderer.Draw(UI::GetDrawData());
    m_Renderer.Close();
}
} // namespace Mana
