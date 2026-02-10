#include "Window.h"

namespace Mana {
const Monitor &Window::PickBestMonitor(const std::vector<Monitor> &monitors) {
    int bestScore = 0;
    int bestIdx;

    int score;
    for (int i = 0; i < monitors.size(); i++) {
        score = monitors[i].Height;
        score += monitors[i].RefreshRate;
        score += monitors[i].Primary ? 1 : 0;
        if (score > bestScore) {
            bestScore = score;
            bestIdx = i;
        }
    }

    return monitors[bestIdx];
}
} // namespace Mana
