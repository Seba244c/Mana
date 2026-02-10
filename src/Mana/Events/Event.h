#pragma once
#include <mpch.h>

namespace Mana {
enum class EventType {
    None = 0,
    WindowClose,
    WindowResize,
    WindowFramebufferResize,
    WindowFocus,
    WindowLostFocus,
    WindowMoved,

    KeyPressed,
    KeyReleased,
    KeyTyped, // TODO: Actually implement with UTF8 support
    ButtonPressed,
    ButtonReleased,
    MouseMoved,
    MouseSrolled,
    ControllerConnected,
    ControllerDisconnected,
    Last = ControllerDisconnected
};

enum EventCatagory {
    None = 0,
    CatagoryApplication = BIT(0),
    CatagoryIO = BIT(1),
    CatagoryKeyboard = BIT(2),
    CatagoryMouse = BIT(3),
    CatagoryMouseButton = BIT(4),
    CatagoryController = BIT(5)
};

// Used by event subclasses
#define EVENT_CLASS_CATEGORY(category)                                         \
    virtual int Category() const override { return category; }

#define EVENT_CLASS_TYPE(type)                                                 \
    static Mana::EventType GetStaticType() { return EventType::type; }         \
    [[nodiscard]] Mana::EventType GetEventType() const override {              \
        return GetStaticType();                                                \
    }                                                                          \
    [[nodiscard]] const char *Name() const override { return #type; }

class Event {
  public:
    virtual ~Event() = default;
    bool Handled = false;

    [[nodiscard]] virtual int Category() const = 0;
    [[nodiscard]] virtual EventType GetEventType() const = 0;
    [[nodiscard]] virtual std::string ToString() const { return Name(); };
    [[nodiscard]] virtual const char *Name() const = 0;

    [[nodiscard]] inline bool IsInCategory(const EventCatagory category) const {
        return Category() & category;
    }

    static const char *GetEventTypeName(EventType type) {
        switch (type) {
        case EventType::WindowClose:
            return "WindowClose";
        case EventType::WindowResize:
            return "WindowResize";
        case EventType::WindowFocus:
            return "WindowFocus";
        case EventType::WindowLostFocus:
            return "WindowLostFocus";
        case EventType::WindowMoved:
            return "WindowMoved";
        case EventType::KeyPressed:
            return "KeyPressed";
        case EventType::KeyReleased:
            return "KeyReleased";
        case EventType::KeyTyped:
            return "KeyTyped";
        case EventType::ButtonPressed:
            return "ButtonPressed";
        case EventType::ButtonReleased:
            return "ButtonReleased";
        case EventType::MouseMoved:
            return "MouseMoved";
        case EventType::MouseSrolled:
            return "MouseSrolled";
        case EventType::ControllerConnected:
            return "ControllerConnected";
        case EventType::ControllerDisconnected:
            return "ControllerDisconnected";
        default:
            return "Unknown";
        }
    }
}; // namespace Mana

inline std::ostream &operator<<(std::ostream &os, const Event &e) {
    return os << e.ToString();
}

class EventDispatcher {
  public:
    explicit EventDispatcher(Event *event) : m_Event(event) {}

    // F (specific eventtypes) will be deduced by the compiler
    // Returns true if handled by the funtion
    template <typename T, typename F> bool Dispatch(const F &func) {
        if (m_Event->GetEventType() == T::GetStaticType()) {
            m_Event->Handled |= func(static_cast<T &>(*m_Event));
            return true;
        }
        return false;
    }

  private:
    Event *m_Event;
};

typedef std::function<void(void *ptr, Event *e)> ObserverCallback;

class EventBridge {
  public:
    static void CallObseverComponents(Event *e) {
        for (auto &componentType : s_Observing[e->GetEventType()]) {
            for (auto &instance : componentType.second) {
                componentType.first(instance, e);
            }
        }
    }

    template <typename T>
    static void ObserveEvent(flecs::world &world, EventType type,
                             ObserverCallback callback) {
        s_Observing[type].emplace_back(callback, std::vector<void *>{});
        auto &instances =
            s_Observing[type].back().second; // reference to stored vector

        world.observer<T>()
            .event(flecs::OnAdd)
            .each([&instances](flecs::entity e, T &t) {
                instances.push_back(&t);
            });

        world.observer<T>()
            .event(flecs::OnRemove)
            .each([&instances](flecs::entity e, T &t) {
                auto it = std::find(instances.begin(), instances.end(), &t);
                if (it != instances.end()) {
                    instances.erase(it); // Remove the instance from the vector
                }
            });
    }

  private:
    static std::map<
        EventType,
        std::vector<std::pair<ObserverCallback, std::vector<void *>>>>
        s_Observing;
};
} // namespace Mana
