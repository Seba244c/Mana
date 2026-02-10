#pragma once
#include "mpch.h"

#include "Mana/Events/Event.h"
#include "Mana/Events/IOEvents.h"
#include "Mana/IO/KeyCodes.h"
#include "Mana/IO/MouseCodes.h"

namespace Mana {
struct ControllerState {
    bool Connected = false;

    Scope<Controller> Controller = nullptr;

    // Delete copy to make sure the Scope<Controller> is not messed with
    ControllerState() = default;
    ControllerState(const ControllerState &) = delete;
    ControllerState &operator=(const ControllerState &) = delete;
};

class Input {
  public:
    static glm::vec2 InputVector(Key::KeyCode up, Key::KeyCode down,
                                 Key::KeyCode left, Key::KeyCode right);
    static glm::vec2 InputVector(ControllerAxis updown,
                                 ControllerAxis leftright, ControllerID cId);
    static glm::vec2 InputVector(Key::KeyCode up, Key::KeyCode down,
                                 Key::KeyCode left, Key::KeyCode right,
                                 ControllerAxis ud, ControllerAxis lr,
                                 ControllerID cId);

    static bool IsKeyPressed(Key::KeyCode key);
    static bool IsKeyDown(Key::KeyCode key);

    static bool IsMouseButtonPressed(Mouse::MouseCode button);
    static bool IsMouseButtonDown(Mouse::MouseCode button);
    static glm::vec2 GetMousePosition();
    static float GetMouseX();
    static float GetMouseY();
    static const Scope<Controller> &Controller(ControllerID);
    static void NewFrame();

    static void Event(Event &e);

  protected:
    static bool OnMouseButtonPressed(const ButtonPressedEvent &e);
    static bool OnMouseButtonReleased(const ButtonReleasedEvent &e);
    static bool OnMouseMoved(const MouseMovedEvent &e);
    static bool OnKeyPressed(const KeyPressedEvent &e);
    static bool OnKeyReleased(const KeyReleasedEvent &e);
    static bool OnControllerConnect(ControllerConnectedEvent &e);
    static bool OnControllerDisconnect(const ControllerDisconnectedEvent &e);
};
} // namespace Mana
