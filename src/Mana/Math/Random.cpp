#include "Random.h"
#include <random>

namespace Mana {
static std::random_device rd;
static std::mt19937 s_Generator(rd());
static std::uniform_real_distribution s_FloatDistribution(0.0f, 1.0f); // Float

float Random::Float() { return s_FloatDistribution(s_Generator); }
float Random::Float(const float min, const float max) {
    return min + (max - min) * Float();
}
int Random::Int(const int min, const int max) {
    std::uniform_int_distribution<> intDistribution(min, max);
    return intDistribution(s_Generator);
}
} // namespace Mana