#include "Math.h"

namespace Mana {
bool Math::PointInRect(glm::vec2 point, glm::vec2 pos, glm::vec2 size) {
    return point.x >= pos.x && point.x <= (pos.x + size.x) &&
           point.y >= pos.y && point.y <= (pos.y + size.y);
}
} // namespace Mana
