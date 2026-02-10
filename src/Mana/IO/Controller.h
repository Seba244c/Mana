#pragma once
#include <mpch.h>

namespace Mana {
typedef uint8_t ControllerID;
typedef uint8_t ControllerBackendID;

using ControllerButton = uint8_t;
enum ControllerButtons : ControllerButton {
    A = 0,
    B = 1,
    X = 2,
    Y = 3,

    L_BUMPER = 4,
    R_BUMPER = 5,

    BACK = 6,
    START = 7,
    GUIDE = 8,

    L_THUMB = 9,
    R_THUMB = 10,

    DPAD_UP = 11,
    DPAD_RIGHT = 12,
    DPAD_DOWN = 13,
    DPAD_LEFT = 14,

    CROSS = A,
    CIRCLE = B,
    SQUARE = X,
    TRIANGLE = Y,
};
#define M_CONTROLLER_BUTTON_LAST 14;

using ControllerAxis = uint8_t;
enum ControllerAxes : ControllerAxis {
    LEFT_X = 0,
    LEFT_Y = 1,
    RIGHT_X = 2,
    RIGHT_Y = 3,

    L_TRIGGER = 4,
    R_TRIGGER = 5,
};
#define M_CONTROLLER_AXIS_LAST 5;

class Controller {
  public:
    virtual ~Controller() = default;
    virtual void PollState() = 0;
    [[nodiscard]] virtual std::string_view Name() const = 0;
    [[nodiscard]] virtual bool Button(ControllerButton b) const = 0;
    [[nodiscard]] virtual float Axis(ControllerAxis b) const = 0;
};
} // namespace Mana
