#include "Layer.h"

#include "Mana/Core/Application.h"

namespace Mana {
Layerstack::Layerstack() = default;
Layerstack::~Layerstack() {
    for (Layer *layer : m_Layers) {
        layer->OnDetach();
        delete layer;
    }
}

void Layerstack::PushLayer(Layer *layer) {
    M_ASSERT(layer != nullptr, "Layer must not be a nullptr!");
    layer->Size(Application::Instance()
                    ->GetWindow()
                    .GetWindowSize()); // The might be added during runtime, and
                                       // therefore be sized {100, 100}
    layer->OnAttach();
    m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
    m_LayerInsertIndex++;
}

void Layerstack::PushOverlay(Layer *overlay) {
    M_ASSERT(overlay != nullptr, "Layer must not be a nullptr!");
    overlay->Size(Application::Instance()
                      ->GetWindow()
                      .GetWindowSize()); // The might be added during runtime,
                                         // and therefore be sized {100, 100}
    overlay->OnAttach();
    m_Layers.emplace_back(overlay);
}

Layer *Layerstack::PopLayer(Layer *layer) {
    if (auto it = std::find(m_Layers.begin(),
                            m_Layers.begin() + m_LayerInsertIndex, layer);
        it != m_Layers.begin() + m_LayerInsertIndex) {
        layer->OnDetach();
        m_Layers.erase(it);
        m_LayerInsertIndex--;
        return layer;
    }

    return nullptr;
}

Layer *Layerstack::PopOverlay(Layer *overlay) {
    if (auto it = std::find(m_Layers.begin() + m_LayerInsertIndex,
                            m_Layers.end(), overlay);
        it != m_Layers.end()) {
        overlay->OnDetach();
        m_Layers.erase(it);
        return overlay;
    }

    return nullptr;
}
} // namespace Mana
