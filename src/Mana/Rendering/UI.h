#pragma once
#include "mpch.h"

#include "Mana/Events/Event.h"
#include "Mana/Events/IOEvents.h"
#include "Mana/IO/Window.h"
#include "Mana/Rendering/Font.h"
#include "Mana/Rendering/Renderer.h"

#include <string_view>

namespace Mana {
class Layer;
struct UIContraints {
    glm::vec2 Pos;
    glm::vec2 Size;
};

class Widget {
    friend class UI;

  protected:
    glm::vec2 p_Position{0, 0};
    glm::vec2 p_Size{100.0f};
    std::vector<Widget *> m_Children;

  public:
    virtual ~Widget() {
        for (auto child : m_Children) {
            delete child;
        }
        m_Children.clear();
    }
    const glm::vec2 &GetPosition() { return p_Position; }
    Widget &Position(glm::vec2 pos) {
        p_Position = pos;

        Dirty = true;
        return *this;
    }
    const glm::vec2 &GetSize() { return p_Size; }
    Widget &Size(glm::vec2 size) {
        p_Size = size;

        Dirty = true;
        return *this;
    }
    Widget &FillParent();
    Widget &Children(std::function<void()> contents);

  protected:
    virtual bool OnPressed(const glm::vec2 pos) {
        // MC_TRACE("Pressed at {:.0f}x{:.0f}+", pos.x, pos.y);
        for (Widget *child : m_Children) {
            glm::vec2 local = pos - child->GetPosition();
            // MC_TRACE("Child at {:.0f}x{:.0f}, Size {:.0f}x{:.0f}+",
            // child->p_Position.x, child->p_Position.y, child->p_Size.x,
            // child->p_Size.y);
            if (glm::all(glm::greaterThanEqual(local, glm::vec2(0.0f))) &&
                glm::all(glm::lessThanEqual(local, child->GetSize()))) {
                if (child->OnPressed(local)) {
                    // MC_TRACE("Returned true-");
                    return true;
                }
                // MC_TRACE("Returned false-");
            } else {
                // MC_TRACE("Skipped-");
            }
        }
        // MC_TRACE("-");

        return false;
    }

  public:
    bool IsDirty();
    void PushDrawData(Renderer2D::DrawData &out, UIContraints constraints);
    virtual void Draw(Renderer2D::DrawData &out, UIContraints constraints) = 0;
    std::vector<Widget *> &GetChildren() { return m_Children; };

  protected:
    bool Dirty = true;

  private:
    Renderer2D::DrawData m_Draw;
};

class Container : public Widget {
    void Draw(Renderer2D::DrawData &out, UIContraints constraints) override {}
};

class HorizontalAlign : public Widget {
  public:
    void Draw(Renderer2D::DrawData &out, UIContraints constraints) override {
        for (Widget *child : m_Children) {
            auto ptr =
                reinterpret_cast<HorizontalAlign *>(child); // Is this legal?
            ptr->p_Position.x =
                p_Position.x + (p_Size.x - ptr->p_Size.x) * 0.5f;
        }
    }
};

class Box : public Widget {
  protected:
    Color p_Color = Colors::White;
    Texture *p_Texture = nullptr;

  public:
    Widget &Color(Color c) {
        p_Color = c;

        Dirty = true;
        return *this;
    }

    Box &Texture(const Ref<Texture> &t) {
        p_Texture = t.get();

        Dirty = true;
        return *this;
    }

  public:
    void Draw(Renderer2D::DrawData &out, UIContraints constraints) override {
        glm::vec2 bl = p_Position + constraints.Pos;
        out.Quads.push_back({bl,
                             {bl.x + p_Size.x, bl.y},
                             bl + p_Size,
                             {bl.x, bl.y + p_Size.y},
                             p_Texture,
                             p_Color});
    }

  protected:
    bool OnPressed(const glm::vec2 pos) override {
        Widget::OnPressed(pos);
        return true;
    }
};

class Button : public Box {
  protected:
    std::function<void()> p_OnClicked = nullptr;

  public:
    Button &OnClicked(std::function<void()> onClicked) {
        p_OnClicked = onClicked;
        Dirty = true;
        return *this;
    }

  protected:
    bool OnPressed(const glm::vec2 pos) override {
        if (Widget::OnPressed(pos)) // If a child is pressed
            return true;

        if (p_OnClicked) {
            p_OnClicked();
        } else {
            MC_WARN("No OnClicked function given to button");
        }

        return true;
    }
};

class SpriteBox : public Box {
  protected:
    std::array<glm::vec2, 4> texCoords;

  public:
    SpriteBox &TexCoords(glm::vec2 min, glm::vec2 max) {
        texCoords[0] = {min.x, min.y};
        texCoords[1] = {max.x, min.y};
        texCoords[2] = {max.x, max.y};
        texCoords[3] = {min.x, max.y};

        Dirty = true;
        return *this;
    }

    SpriteBox &TexCoords(glm::vec4 coords) {
        texCoords[0] = {coords.x, coords.y};
        texCoords[1] = {coords.z, coords.y};
        texCoords[2] = {coords.z, coords.w};
        texCoords[3] = {coords.x, coords.w};

        Dirty = true;
        return *this;
    }

  public:
    void Draw(Renderer2D::DrawData &out, UIContraints constraints) override {
        glm::vec2 bl = p_Position + constraints.Pos;
        out.Quads.push_back(Renderer2D::Quad{bl,                      // bl
                                             {bl.x + p_Size.x, bl.y}, // br
                                             bl + p_Size,             // tr
                                             {bl.x, bl.y + p_Size.y}, // tl
                                             p_Texture,               // texture
                                             p_Color,                 // Color
                                             texCoords});
    }
};

class Text : public Widget {
  protected:
    Color p_Color = Colors::White;
    Font *p_Font = nullptr;
    float p_FontSize = 24.0f;
    std::string p_String;

  public:
    Text &Color(Color c) {
        p_Color = c;

        Dirty = true;
        return *this;
    }

    Text &Font(const Ref<Font> &t) {
        p_Font = t.get();
        UpdateSize();

        Dirty = true;
        return *this;
    }

    Text &String(const std::string_view text) {
        p_String = text;
        UpdateSize();

        Dirty = true;
        return *this;
    }

    Text &Size(const float size) {
        p_FontSize = size;
        UpdateSize();

        Dirty = true;
        return *this;
    }

  private:
    void UpdateSize();

  public:
    void Draw(Renderer2D::DrawData &out, UIContraints constraints) override {
        M_ASSERT(p_Font);
        glm::vec2 bl = p_Position + constraints.Pos;
        out.Labels.push_back(
            Renderer2D::Label{bl, p_String, p_Font, p_FontSize, p_Color});
    }
};

class UI {
  public:
    static void Initialize(Scope<Window> &window);
    static void OnEvent(Event *e);
    static void Clear();
    static void Draw(Layer *layer);
    static Renderer2D::DrawData &GetDrawData();

    static HorizontalAlign *HorizontalAlign();
    static Box *Box();
    static Button *Button();
    static SpriteBox *Sprite();
    static Text *Text(const std::string_view text);
    static const glm::vec2 &ParentSize();
};
} // namespace Mana
