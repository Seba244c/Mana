#include "Event.h"

namespace Mana {
std::map<EventType,
         std::vector<std::pair<ObserverCallback, std::vector<void *>>>>
    EventBridge::s_Observing;
}
