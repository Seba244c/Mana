#pragma once
#include <mpch.h>

#include "Mana/Audio/Sound.h"

namespace Mana {
class Hit {
  public:
    Hit() {}
    Hit(Ref<Sound> source, ma_sound *sound, ma_audio_buffer *buffer)
        : Source(source), Sound(sound), Buffer(buffer) {}

    Ref<Sound> Source;
    ma_sound *Sound;
    ma_audio_buffer *Buffer;
};

class AudioChannel {
    friend class AudioManager;

  public:
    float GetVolume();
    void SetVolume(const float volume);

  protected:
    AudioChannel(ma_engine *MAEngine);
    ~AudioChannel();

    ma_sound_group m_MAChannel;

    std::mutex m_SoundsMutex;
    std::vector<Hit> m_Hits;
};

class AudioManager {
  public:
    AudioManager();
    ~AudioManager();

    AudioChannel *RegisterChannel();
    void DeleteChannel(AudioChannel *channel);
    void PlaySound(AudioChannel *channel, const Ref<Sound> &sound);

    float GetMasterVolume();
    void SetMasterVolume(float value);

  private:
    void AudioThread();

  private:
    ma_engine m_MAEngine;
    ma_resource_manager m_Resources;

    std::vector<AudioChannel *> m_Channels;
};
} // namespace Mana
