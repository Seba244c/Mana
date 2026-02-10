#pragma once
#include "mpch.h"

namespace Mana {
class Random {
  public:
    [[nodiscard]] static float Float();
    [[nodiscard]] static float Float(float min, float max);
    [[nodiscard]] static int Int(int min, int max);
};
} // namespace Mana