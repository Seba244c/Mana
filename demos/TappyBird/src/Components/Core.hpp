#pragma once

#include <Mana.h>

namespace Game {
struct Death {};
struct Point {
    bool Scored = false;
};
struct Score {
    uint Score = 0;
};

static Mana::AudioChannel *s_ChannelSFX;
static Mana::AudioChannel *s_ChannelMusic;

inline void InitAudio() {
    s_ChannelSFX = Mana::Application::Instance()->GetAudio().RegisterChannel();
    s_ChannelMusic =
        Mana::Application::Instance()->GetAudio().RegisterChannel();
}
} // namespace Game
