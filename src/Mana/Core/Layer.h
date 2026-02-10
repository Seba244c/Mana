#pragma once
#include <mpch.h>

#include "Mana/Core/Platform.h"
#include "Mana/Events/Event.h"
#include "Mana/IO/Input.h"
#include "Mana/Math/Math.h"
#include "Mana/Rendering/UI.h"

namespace Mana {
class Renderer2D;
class Layer : public Mana::Widget {
  public:
    explicit Layer(const std::string &name = "Layer") : m_Name(name) {}

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void Update(const Time::Duration &delta) {}
    virtual void OnEvent(Event &event) {
        EventDispatcher dispatcher(&event);
        dispatcher.Dispatch<ButtonPressedEvent>(
            M_BIND_EVENT_FN(OnPressedEvent));
    }

    bool OnPressedEvent(const ButtonPressedEvent &event) {
        glm::vec2 pos = Input::GetMousePosition();
        pos.y = p_Size.y - pos.y; // Converting to UI space, where origin (0,0)
                                  // is at the BOTTOM left corner
        return OnPressed(pos);
    }

    virtual void Draw() {}
    void Draw(Renderer2D::DrawData &out, UIContraints constraints) override {
        Children([this]() { this->Draw(); });
    };

    [[nodiscard]] const std::string &Name() const { return m_Name; }

  protected:
    std::string m_Name;
};

class Layerstack {
  public:
    Layerstack();
    ~Layerstack();

    void PushLayer(Layer *layer);
    void PushOverlay(Layer *overlay);

    Layer *PopLayer(Layer *layer);

    Layer *PopOverlay(Layer *overlay);

    [[nodiscard]] bool IsEmpty() const { return m_Layers.empty(); };

    std::vector<Layer *>::iterator begin() { return m_Layers.begin(); }
    std::vector<Layer *>::iterator end() { return m_Layers.end(); }
    std::vector<Layer *>::reverse_iterator rbegin() {
        return m_Layers.rbegin();
    }
    std::vector<Layer *>::reverse_iterator rend() { return m_Layers.rend(); }

    [[nodiscard]] std::vector<Layer *>::const_iterator begin() const {
        return m_Layers.begin();
    }
    [[nodiscard]] std::vector<Layer *>::const_iterator end() const {
        return m_Layers.end();
    }
    [[nodiscard]] std::vector<Layer *>::const_reverse_iterator rbegin() const {
        return m_Layers.rbegin();
    }
    [[nodiscard]] std::vector<Layer *>::const_reverse_iterator rend() const {
        return m_Layers.rend();
    }

  private:
    std::vector<Layer *> m_Layers;
    uint m_LayerInsertIndex = 0;
};
} // namespace Mana
