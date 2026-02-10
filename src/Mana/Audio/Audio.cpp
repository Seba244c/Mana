#include "Audio.h"

#include "Mana/Audio/Sound.h"
#include "Mana/Core/Application.h"
#include "Mana/Services/Service.h"

namespace Mana {
static constexpr uint8_t s_MaxChannelHits = 32;

AudioChannel::AudioChannel(ma_engine *MAEngine) {
    MC_TRACE("Creating audio channel with support for up to {} hits",
             s_MaxChannelHits);
    ma_sound_group_init(MAEngine, 0, NULL, &m_MAChannel);
}

float AudioChannel::GetVolume() {
    return ma_sound_group_get_volume(&m_MAChannel);
}
void AudioChannel::SetVolume(const float value) {
    M_ASSERT(0.0f <= value && value <= 1.0f,
             "Master volume should follow: 0 <= value <= 1");
    MC_INFO("Setting volume level for channel to {}", value);
    ma_sound_group_set_volume(&m_MAChannel, std::clamp(value, 0.0f, 1.0f));
}

AudioChannel::~AudioChannel() { ma_sound_group_uninit(&m_MAChannel); }

void AudioManager::AudioThread() {
    for (auto channel : m_Channels) {
        auto &mutex = channel->m_SoundsMutex;
        auto &sounds = channel->m_Hits;

        std::lock_guard<std::mutex> lock(mutex);
        std::vector<Hit> toDestroy;
        sounds.erase(std::remove_if(sounds.begin(), sounds.end(),
                                    [](Hit &hit) {
                                        bool destroy =
                                            ma_sound_at_end(hit.Sound) ==
                                            MA_TRUE; // Destruction ensues

                                        if (destroy) {
                                            // Destroy the sound
                                            ma_sound_uninit(hit.Sound);
                                            ma_audio_buffer_uninit(hit.Buffer);

                                            delete hit.Sound;
                                            delete hit.Buffer;
                                        }
                                        return destroy;
                                    }),
                     sounds.end());
    }
}

AudioManager::AudioManager() {
    MC_TRACE("Starting MiniAudio engine");
    ma_result result = ma_engine_init(NULL, &m_MAEngine);

    if (result != MA_SUCCESS) {
        MC_CRITICAL("Failed to start MiniAudio engine! Error: {}", (int)result);
        M_ASSERT(false, "Failed to start audio engine");

        return;
    }

    Application::Instance()->GetServices().Schedule(
        "audio-cleanup", {[this]() { AudioThread(); }, JobPriority::Normal},
        Time::Seconds(4));
}

void AudioManager::PlaySound(AudioChannel *channel, const Ref<Sound> &sound) {
    M_ASSERT(std::find(m_Channels.begin(), m_Channels.end(), channel) !=
                 m_Channels.end(),
             "SHOULD NOT PLAY SOUND TO UNREGISTERED CHANNEL!");
    M_ASSERT(sound != nullptr, "Sound not loaded!");

    // Currently this prevents Hits from being destroyed and recreated when the
    // vector resizes. If an infinite size is needed the Hits should be changed
    // to Scope<Hit>
    if (channel->m_Hits.size() >= s_MaxChannelHits) {
        MC_WARN("Exceeded maximum hits! Ignoring PlaySound request.");
        return;
    }

    // Create a Hit
    auto *dataSource = sound->GetAudioBuffer();
    ma_sound *sound_instance = new ma_sound;
    if (ma_sound_init_from_data_source(
            &m_MAEngine, dataSource,
            MA_SOUND_FLAG_NO_SPATIALIZATION | MA_SOUND_FLAG_NO_PITCH,
            &channel->m_MAChannel, sound_instance) != MA_SUCCESS) {
        MC_WARN("Failed to init sound, deleting and returning!");
        ma_audio_buffer_uninit(dataSource);
        ma_sound_uninit(sound_instance);

        return;
    }

    // Start the sound
    ma_sound_start(sound_instance);

    // Register the hit so we can clean it up later
    std::lock_guard<std::mutex> lock(channel->m_SoundsMutex);
    channel->m_Hits.emplace_back(sound, sound_instance, dataSource);
}

float AudioManager::GetMasterVolume() {
    return ma_engine_get_volume(&m_MAEngine);
}
void AudioManager::SetMasterVolume(float value) {
    M_ASSERT(0.0f <= value && value <= 1.0f,
             "Master volume should follow: 0 <= value <= 1");
    MC_INFO("Setting master volume to {}", value);
    ma_engine_set_volume(&m_MAEngine, std::clamp(value, 0.0f, 1.0f));
}

AudioChannel *AudioManager::RegisterChannel() {
    AudioChannel *channel = new AudioChannel(&m_MAEngine);
    m_Channels.push_back(channel);

    return channel;
}

void AudioManager::DeleteChannel(AudioChannel *channel) {
    // TODO: FIX FOR WHEN CHANNELS GET MISUSED!!
    m_Channels.erase(std::find(m_Channels.begin(), m_Channels.end(), channel));
    delete channel;
}

AudioManager::~AudioManager() {
    Application::Instance()->GetServices().CancelScheduled("audio-cleanup");

    MC_TRACE("Stopping MiniAudio engine");
    ma_engine_uninit(&m_MAEngine);

    for (auto channel : m_Channels) {
        std::lock_guard<std::mutex> lock(channel->m_SoundsMutex);
        channel->m_Hits.clear(); // Everything is destructed
    }
}
} // namespace Mana
