#include <bitset>
#include <glm/ext/scalar_constants.hpp>

#include "Input.h"
#include "Mana/Events/IOEvents.h"

namespace Mana {
static std::bitset<Key::LAST + 1> s_KeyPressed;
static std::bitset<Mouse::ButtonLast + 1> s_MousePressed;
static std::bitset<Key::LAST + 1> s_KeyDown;
static std::bitset<Mouse::ButtonLast + 1> s_MouseDown;
static glm::vec2 s_Mouse;
static std::array<ControllerState, 16> s_Controllers;
static std::array<ControllerID, 16>
    s_ControllerIDMap; // The index is the backend id for a controller (e.g.
                       // GLFW), and the value is the index in the s_Controllers
                       // array

const Scope<Controller> &Input::Controller(const ControllerID cId) {
    return s_Controllers[cId].Controller;
}

glm::vec2 Input::InputVector(Key::KeyCode up, Key::KeyCode down,
                             Key::KeyCode left, Key::KeyCode right) {
    glm::vec2 out = glm::vec2(s_KeyDown[right] - s_KeyDown[left],
                              s_KeyDown[up] - s_KeyDown[down]);

    if (length(out) <= glm::epsilon<float>())
        return out;

    return glm::normalize(out);
}
glm::vec2 Input::InputVector(ControllerAxis updown, ControllerAxis leftright,
                             ControllerID cId) {
    if (!s_Controllers[cId].Connected)
        return {0, 0};
    glm::vec2 out = {Controller(cId)->Axis(leftright),
                     -Controller(cId)->Axis(updown)};
    if (length(out) < 0.1f) // TODO: THIS VALUE IS ARBITRARY
        return {0, 0};
    // Don't normalize, since we assume it's always below 1, and we want vectors
    // of length between 0-1

    return out;
}

glm::vec2 Input::InputVector(Key::KeyCode up, Key::KeyCode down,
                             Key::KeyCode left, Key::KeyCode right,
                             ControllerAxis ud, ControllerAxis lr,
                             ControllerID cId) {
    glm::vec2 out = InputVector(up, down, left, right);
    out += InputVector(ud, lr, cId);

    if (length(out) >
        1) // Controllers might return a vector that is less than 1
        return normalize(out);
    return out;
}

bool Input::IsKeyPressed(const Key::KeyCode key) { return s_KeyPressed[key]; }

bool Input::IsMouseButtonPressed(const Mouse::MouseCode button) {
    return s_MousePressed[button];
}

bool Input::IsKeyDown(const Key::KeyCode key) { return s_KeyDown[key]; }

bool Input::IsMouseButtonDown(const Mouse::MouseCode button) {
    return s_MouseDown[button];
}

glm::vec2 Input::GetMousePosition() { return s_Mouse; }

float Input::GetMouseX() { return s_Mouse.x; }

float Input::GetMouseY() { return s_Mouse.y; }

void Input::Event(Mana::Event &e) {
    if (!e.IsInCategory(CatagoryIO))
        return;
    EventDispatcher dispatcher(&e);
    dispatcher.Dispatch<KeyPressedEvent>(
        M_BIND_EVENT_STATIC_FN(Input::OnKeyPressed));
    dispatcher.Dispatch<KeyReleasedEvent>(
        M_BIND_EVENT_STATIC_FN(Input::OnKeyReleased));
    dispatcher.Dispatch<MouseMovedEvent>(
        M_BIND_EVENT_STATIC_FN(Input::OnMouseMoved));
    dispatcher.Dispatch<ButtonPressedEvent>(
        M_BIND_EVENT_STATIC_FN(Input::OnMouseButtonPressed));
    dispatcher.Dispatch<ButtonReleasedEvent>(
        M_BIND_EVENT_STATIC_FN(Input::OnMouseButtonReleased));
    dispatcher.Dispatch<ControllerConnectedEvent>(
        M_BIND_EVENT_STATIC_FN(Input::OnControllerConnect));
    dispatcher.Dispatch<ControllerDisconnectedEvent>(
        M_BIND_EVENT_STATIC_FN(Input::OnControllerDisconnect));
}

void Input::NewFrame() {
    s_MousePressed.reset();
    s_KeyPressed.reset();

    for (ControllerID c = 0; c < 16; c++) {
        if (s_Controllers[c].Connected)
            s_Controllers[c].Controller->PollState();
    }
}

bool Input::OnMouseButtonPressed(const ButtonPressedEvent &e) {
    s_MouseDown[e.MouseButton()] = true;
    s_MousePressed[e.MouseButton()] = true;
    return false;
}

bool Input::OnMouseButtonReleased(const ButtonReleasedEvent &e) {
    s_MouseDown[e.MouseButton()] = false;
    return false;
}

bool Input::OnMouseMoved(const MouseMovedEvent &e) {
    s_Mouse.x = e.X();
    s_Mouse.y = e.Y();
    return false;
}

bool Input::OnKeyPressed(const KeyPressedEvent &e) {
    if (e.IsRepeat())
        return false;

    s_KeyDown[e.KeyCode()] = true;
    s_KeyPressed[e.KeyCode()] = true;
    return false;
}

bool Input::OnKeyReleased(const KeyReleasedEvent &e) {
    s_KeyDown[e.KeyCode()] = false;
    return false;
}

ControllerID FindFirstAvailableContrller() {
    for (ControllerID i = 0; i < 16; i++) {
        if (!s_Controllers[i].Connected)
            return i;
    }

    MC_ERROR("Controller capacity exceeded. Returning as last controller");
    return 15;
}

bool Input::OnControllerConnect(ControllerConnectedEvent &e) {
    M_ASSERT(e.ControllerBackendID() < 16,
             "Invalid backend controller id! Backend controller id's must be "
             "between 0-15");

    auto cId = FindFirstAvailableContrller();
    s_ControllerIDMap[e.ControllerBackendID()] = cId;

    auto &state = s_Controllers[cId];
    state.Connected = true;
    state.Controller = std::move(e.Controller);

    MC_INFO("Controller {} (Id: {}, Backend: {}) connected.",
            state.Controller->Name(), cId, e.ControllerBackendID());

    return false;
}

bool Input::OnControllerDisconnect(const ControllerDisconnectedEvent &e) {
    M_ASSERT(e.ControllerBackendID() < 16,
             "Invalid backend controller id! Backend controller id's must be "
             "between 0-15");

    auto cId = s_ControllerIDMap[e.ControllerBackendID()];
    auto &state = s_Controllers[cId];
    if (!state.Connected) {
        MC_WARN("Controller {} disconnected, but it was not connected in the "
                "fist place!",
                e.ControllerBackendID());
        return true;
    }

    state.Connected = false;

    MC_INFO("Controller {} (Id: {}, Backend: {}) disconnected.",
            state.Controller->Name(), cId, e.ControllerBackendID());

    return false;
}
} // namespace Mana
