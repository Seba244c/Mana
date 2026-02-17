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

/** \brief Static global input state getters.
 * */
class Input {
  public:
    /** Calculates a normalized input vector, where left is (-1, 0), right (1,
     * 0), up (0, 1) and down (0, -1), based on which keys are pressed.
     * The returned vector may have the length 0, if none of the keys are
     * pressed, or 1.
     * */
    static glm::vec2 InputVector(Key::KeyCode up, Key::KeyCode down,
                                 Key::KeyCode left, Key::KeyCode right);

    /** Calculates a normalized input vector, where left is (-1, 0), right (1,
     * 0), up (0, 1) and down (0, -1), based on a joystick.
     * The returned vector may have the length 0, if none of the keys are
     * pressed, or 1.
     * */
    static glm::vec2 InputVector(ControllerAxis updown,
                                 ControllerAxis leftright, ControllerID cId);

    /** Calculates a normalized input vector, where left is (-1, 0), right (1,
     * 0), up (0, 1) and down (0, -1), as the normalized vector, of the sum of
     * InputVector(up, down, left, right) and InputVector(ud, lr, cId).
     * The returned vector may have the length 0, if none of the keys are
     * pressed, or 1.
     * */
    static glm::vec2 InputVector(Key::KeyCode up, Key::KeyCode down,
                                 Key::KeyCode left, Key::KeyCode right,
                                 ControllerAxis ud, ControllerAxis lr,
                                 ControllerID cId);

    /** Returns whether the key was pressed just this frame */
    static bool IsKeyPressed(Key::KeyCode key);

    /** Returns whether the key is currently pressed down */
    static bool IsKeyDown(Key::KeyCode key);

    /** Returns whether the mouse button was pressed just this frame */
    static bool IsMouseButtonPressed(Mouse::MouseCode button);

    /** Returns whether the mouse key is currently pressed down */
    static bool IsMouseButtonDown(Mouse::MouseCode button);

    /** Returns the current mouse position, in the virtual coordinate space of
     * the window system. (0,0) is top left, (width,height) is bottom right.
     * */
    static glm::vec2 GetMousePosition();

    /** Returns the current mouse X position, in the virtual coordinate space of
     * the window system. 0 is the left most coordinate
     * */
    static float GetMouseX();

    /** Returns the current mouse Y position, in the virtual coordinate space of
     * the window system. 0 is the top most coordinate
     * */
    static float GetMouseY();
    static const Scope<Controller> &Controller(ControllerID);

    /** Prepares the state for a new frame. Should only be called by Mana
     * internally */
    static void NewFrame();

    /** Updates input state based on an event. Should only be called by Mana
     * internally */
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
