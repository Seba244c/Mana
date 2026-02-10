#pragma once
#include <mpch.h>

#include "Event.h"

#include "Mana/IO/Controller.h"
#include "Mana/IO/KeyCodes.h"
#include "Mana/IO/MouseCodes.h"

namespace Mana {
class WindowResizeEvent final : public Event {
  public:
    WindowResizeEvent(const unsigned int width, const unsigned int height)
        : m_Width(width), m_Height(height) {}

    [[nodiscard]] unsigned int GetWidth() const { return m_Width; }
    [[nodiscard]] unsigned int GetHeight() const { return m_Height; }

    [[nodiscard]] std::string ToString() const override {
        std::stringstream ss;
        ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
        return ss.str();
    }

    EVENT_CLASS_TYPE(WindowResize)
    EVENT_CLASS_CATEGORY(CatagoryIO)
  private:
    unsigned int m_Width, m_Height;
};

class WindowFramebufferResizeEvent final : public Event {
  public:
    WindowFramebufferResizeEvent(const unsigned int width,
                                 const unsigned int height)
        : m_Width(width), m_Height(height) {}

    [[nodiscard]] unsigned int GetWidth() const { return m_Width; }
    [[nodiscard]] unsigned int GetHeight() const { return m_Height; }

    [[nodiscard]] std::string ToString() const override {
        std::stringstream ss;
        ss << "WindowFramebufferResizeEvent: " << m_Width << ", " << m_Height;
        return ss.str();
    }

    EVENT_CLASS_TYPE(WindowFramebufferResize)
    EVENT_CLASS_CATEGORY(CatagoryIO)
  private:
    unsigned int m_Width, m_Height;
};

class WindowMovedEvent final : public Event {
  public:
    WindowMovedEvent(const int xpos, const int ypos) : m_X(xpos), m_Y(ypos) {}

    [[nodiscard]] unsigned int GetX() const { return m_X; }
    [[nodiscard]] unsigned int GetY() const { return m_Y; }

    [[nodiscard]] std::string ToString() const override {
        std::stringstream ss;
        ss << "WindowMovedEvent: " << m_X << ", " << m_Y;
        return ss.str();
    }

    EVENT_CLASS_TYPE(WindowMoved)
    EVENT_CLASS_CATEGORY(CatagoryIO)
  private:
    unsigned int m_X, m_Y;
};

class WindowCloseEvent final : public Event {
  public:
    WindowCloseEvent() = default;

    EVENT_CLASS_TYPE(WindowClose)
    EVENT_CLASS_CATEGORY(CatagoryIO)
};

class WindowFocusEvent final : public Event {
  public:
    WindowFocusEvent() = default;

    EVENT_CLASS_TYPE(WindowFocus)
    EVENT_CLASS_CATEGORY(CatagoryIO)
};

class WindowLostFocusEvent final : public Event {
  public:
    WindowLostFocusEvent() = default;

    EVENT_CLASS_TYPE(WindowLostFocus)
    EVENT_CLASS_CATEGORY(CatagoryIO)
};

class KeyEvent : public Event {
  public:
    [[nodiscard]] Key::KeyCode KeyCode() const { return m_KeyCode; }
    EVENT_CLASS_CATEGORY(CatagoryKeyboard | CatagoryIO)
  protected:
    explicit KeyEvent(const Key::KeyCode keycode) : m_KeyCode(keycode) {}

    Key::KeyCode m_KeyCode;
};

class KeyPressedEvent final : public KeyEvent {
  public:
    explicit KeyPressedEvent(const Key::KeyCode keycode,
                             const bool isRepeat = false)
        : KeyEvent(keycode), m_IsRepeat(isRepeat) {}

    [[nodiscard]] bool IsRepeat() const { return m_IsRepeat; }

    [[nodiscard]] std::string ToString() const override {
        std::stringstream ss;
        ss << "KeyPressedEvent: " << m_KeyCode << " (repeat = " << m_IsRepeat
           << ")";
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyPressed)
  private:
    bool m_IsRepeat;
};

class KeyReleasedEvent final : public KeyEvent {
  public:
    explicit KeyReleasedEvent(const Key::KeyCode keycode) : KeyEvent(keycode) {}

    [[nodiscard]] std::string ToString() const override {
        std::stringstream ss;
        ss << "KeyReleasedEvent: " << m_KeyCode;
        return ss.str();
    }

    EVENT_CLASS_TYPE(KeyReleased)
};

class KeyTypedEvent final : public KeyEvent {
  public:
    explicit KeyTypedEvent(const Key::KeyCode keycode) : KeyEvent(keycode) {};

    [[nodiscard]] std::string ToString() const override {
        std::stringstream ss;
        ss << "KeyTypedEvent: " << m_KeyCode;
        return ss.str();
    }
    EVENT_CLASS_TYPE(KeyTyped)
};

// ------ MOUSE ------
class MouseMovedEvent final : public Event {
  public:
    MouseMovedEvent(const float x, const float y, const float dx,
                    const float dy)
        : m_MouseX(x), m_MouseY(y), m_DeltaX(dx), m_DeltaY(dy) {}

    [[nodiscard]] float X() const { return m_MouseX; }
    [[nodiscard]] float Y() const { return m_MouseY; }
    [[nodiscard]] float DeltaX() const { return m_DeltaX; }
    [[nodiscard]] float DeltaY() const { return m_DeltaY; }

    [[nodiscard]] std::string ToString() const override {
        std::stringstream ss;
        ss << "MouseMovedEvent: " << m_MouseX << " +(" << m_DeltaX << ")"
           << ", " << m_MouseY << " +(" << m_DeltaY << ")";
        return ss.str();
    }

    EVENT_CLASS_TYPE(MouseMoved);
    EVENT_CLASS_CATEGORY(CatagoryMouse | CatagoryIO);

  private:
    float m_MouseX, m_MouseY, m_DeltaX, m_DeltaY;
};

class MouseScrolledEvent final : public Event {
  public:
    MouseScrolledEvent(const float xOffset, const float yOffset)
        : m_OffsetX(xOffset), m_OffsetY(yOffset) {}

    [[nodiscard]] float XOffset() const { return m_OffsetX; }
    [[nodiscard]] float YOffset() const { return m_OffsetY; }

    [[nodiscard]] std::string ToString() const override {
        std::stringstream ss;
        ss << "MouseScrolledEvent: " << XOffset() << ", " << YOffset();
        return ss.str();
    }

    EVENT_CLASS_TYPE(MouseSrolled);
    EVENT_CLASS_CATEGORY(CatagoryMouse | CatagoryIO);

  private:
    const float m_OffsetX, m_OffsetY;
};

class MouseButtonEvent : public Event {
  public:
    [[nodiscard]] Mouse::MouseCode MouseButton() const { return m_Button; }

    EVENT_CLASS_CATEGORY(CatagoryMouse | CatagoryIO | CatagoryMouseButton);

  protected:
    explicit MouseButtonEvent(const Mouse::MouseCode button)
        : m_Button(button) {}

    Mouse::MouseCode m_Button;
};

class ButtonPressedEvent final : public MouseButtonEvent {
  public:
    explicit ButtonPressedEvent(const Mouse::MouseCode button)
        : MouseButtonEvent(button) {}

    [[nodiscard]] std::string ToString() const override {
        std::stringstream ss;
        ss << "ButtonPressedEvent: " << m_Button;
        return ss.str();
    }

    EVENT_CLASS_TYPE(ButtonPressed)
};

class ButtonReleasedEvent final : public MouseButtonEvent {
  public:
    explicit ButtonReleasedEvent(const Mouse::MouseCode button)
        : MouseButtonEvent(button) {}

    [[nodiscard]] std::string ToString() const override {
        std::stringstream ss;
        ss << "ButtonReleasedEvent: " << m_Button;
        return ss.str();
    }

    EVENT_CLASS_TYPE(ButtonReleased)
};

class ControllerEvent : public Event {
  protected:
    explicit ControllerEvent(const ControllerBackendID id)
        : m_ControllerBackendID(id) {}
    ControllerBackendID m_ControllerBackendID;

  public:
    [[nodiscard]] const ControllerID ControllerBackendID() const {
        return m_ControllerBackendID;
    }

    EVENT_CLASS_CATEGORY(EventCatagory::CatagoryIO |
                         EventCatagory::CatagoryController)
};

class ControllerConnectedEvent final : public ControllerEvent {
  public:
    explicit ControllerConnectedEvent(const ::Mana::ControllerBackendID id,
                                      Scope<Controller> c)
        : ControllerEvent(id), Controller(std::move(c)) {}

    [[nodiscard]] std::string ToString() const override {
        std::stringstream ss;
        ss << "ControllerConnectedevent: " << m_ControllerBackendID << " "
           << Controller->Name();
        return ss.str();
    }

    EVENT_CLASS_TYPE(ControllerConnected);

  public:
    Scope<Controller> Controller;
};

class ControllerDisconnectedEvent final : public ControllerEvent {
  public:
    explicit ControllerDisconnectedEvent(const ::Mana::ControllerBackendID id)
        : ControllerEvent(id) {}

    EVENT_CLASS_TYPE(ControllerDisconnected)
};
} // namespace Mana
