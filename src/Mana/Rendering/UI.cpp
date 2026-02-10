#include "UI.h"

#include "Mana/Core/Core.h"
#include "Mana/Core/Layer.h"
#include "Mana/Events/IOEvents.h"
#include "Mana/IO/MouseCodes.h"

#include <stack>
#include <string_view>

namespace Mana {
struct Context {
    std::stack<Widget *> Containers;
    UIContraints AppConstraints;
    Renderer2D::DrawData DrawData;
};

static Context s_Context;

void UI::Initialize(Scope<Window> &window) {
    // The UI system uses logical pixel size even on Hi-DPI displays for it's
    // coordinate system
    s_Context.AppConstraints.Size = window->GetWindowSize();
}

void UI::Clear() { s_Context.DrawData.Clear(); }
Renderer2D::DrawData &UI::GetDrawData() { return s_Context.DrawData; }
void UI::Draw(Layer *layer) {
    layer->PushDrawData(s_Context.DrawData, s_Context.AppConstraints);
}

bool Widget::IsDirty() {
    for (Widget *w : m_Children) {
        if (w->IsDirty()) {
            Dirty = true;
            return true;
        }
    }

    return Dirty;
}

Widget &Widget::FillParent() {
    Size(UI::ParentSize());
    return *this;
}

Widget &Widget::Children(std::function<void()> contents) {
    if (m_Children.size() > 0) {
        for (auto child : m_Children) {
            delete child;
        }
        m_Children.clear();
    }
    s_Context.Containers.push(this);
    contents();
    s_Context.Containers.pop();

    return *this;
}

void Widget::PushDrawData(Renderer2D::DrawData &out, UIContraints constraints) {
    if (!IsDirty()) {
        // Since this widget didn't change drawdata, we just push the data as is
        out.Quads.insert(out.Quads.end(), m_Draw.Quads.begin(),
                         m_Draw.Quads.end());
        out.Lines.insert(out.Lines.end(), m_Draw.Lines.begin(),
                         m_Draw.Lines.end());
        out.Labels.insert(out.Labels.end(), m_Draw.Labels.begin(),
                          m_Draw.Labels.end());
        return;
    }

    // Since this widget was dirty we need to recalculate the drawdata before
    // pushing it to out parent
    Dirty = false;
    m_Draw.Clear();

    // Draw the actual object
    Draw(m_Draw, constraints);

    // Draw Children
    UIContraints c{constraints.Pos + p_Position,
                   {std::min(p_Size.x, constraints.Size.x),
                    std::min(p_Size.y, constraints.Size.y)}};

    for (Widget *w : m_Children)
        w->PushDrawData(m_Draw, c);
    out.Quads.insert(out.Quads.end(), m_Draw.Quads.begin(), m_Draw.Quads.end());
    out.Lines.insert(out.Lines.end(), m_Draw.Lines.begin(), m_Draw.Lines.end());
    out.Labels.insert(out.Labels.end(), m_Draw.Labels.begin(),
                      m_Draw.Labels.end());
}

// Handle app events

bool OnWindowResize(WindowResizeEvent &e) {
    // The UI system uses logical pixel size even on Hi-DPI displays for it's
    // coordinate system
    MC_TRACE("UI: Resizing parent container {}x{}", e.GetWidth(),
             e.GetHeight());
    s_Context.AppConstraints.Size = {e.GetWidth(), e.GetHeight()};

    return false;
}

bool OnMouseMove(MouseMovedEvent &e) { return false; }

bool OnButtonPressed(ButtonPressedEvent &e) {
    if (e.MouseButton() != Mouse::Button0)
        return false;

    return false;
}

bool OnButtonRelease(ButtonReleasedEvent &e) {
    if (e.MouseButton() != Mouse::Button0)
        return false;

    return false;
}

void UI::OnEvent(Event *event) {
    EventDispatcher e(event);

    e.Dispatch<WindowResizeEvent>(M_BIND_EVENT_STATIC_FN(OnWindowResize));
    e.Dispatch<MouseMovedEvent>(M_BIND_EVENT_STATIC_FN(OnMouseMove));
    e.Dispatch<ButtonPressedEvent>(M_BIND_EVENT_STATIC_FN(OnButtonPressed));
    e.Dispatch<ButtonReleasedEvent>(M_BIND_EVENT_STATIC_FN(OnButtonRelease));
}

HorizontalAlign *UI::HorizontalAlign() {
    s_Context.Containers.top()->GetChildren().push_back(
        new class HorizontalAlign);
    return (class HorizontalAlign *)s_Context.Containers.top()
        ->GetChildren()
        .back();
}

Box *UI::Box() {
    s_Context.Containers.top()->GetChildren().push_back(new class Box);
    return (class Box *)s_Context.Containers.top()->GetChildren().back();
}

Button *UI::Button() {
    s_Context.Containers.top()->GetChildren().push_back(new class Button);
    return (class Button *)s_Context.Containers.top()->GetChildren().back();
}

SpriteBox *UI::Sprite() {
    s_Context.Containers.top()->GetChildren().push_back(new class SpriteBox);
    return (class SpriteBox *)s_Context.Containers.top()->GetChildren().back();
}

Text *UI::Text(const std::string_view text) {
    s_Context.Containers.top()->GetChildren().push_back(new class Text);
    ((class Text *)s_Context.Containers.top()->GetChildren().back())
        ->String(text);
    return (class Text *)s_Context.Containers.top()->GetChildren().back();
}

void Text::UpdateSize() {
    if (p_Font && p_FontSize > 0)
        Renderer2D::LabelWidth(p_String, p_Font, p_FontSize, p_Size.x);
    p_Size.y = p_FontSize;
}

const glm::vec2 &UI::ParentSize() { return s_Context.Containers.top()->p_Size; }
} // namespace Mana
