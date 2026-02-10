#pragma once
#include <mpch.h>

#include "Mana/Core/Asset.h"

namespace Mana {
typedef enum AudioCompressionFormat { RawPCM = 0 } AudioCompressionFormat;

struct SoundMetadataChunk {
    uint64_t FrameCount;
    uint32_t Channels;
    uint32_t SampleRate;
    ma_format DataFormat;
    AudioCompressionFormat CompressionFormat;
};

struct SoundFileData {
    SoundMetadataChunk MetaData;
    std::vector<uint8_t> Data;
};

class Sound {
  public:
    static void CreateMote(std::filesystem::path src,
                           std::vector<MoteChunkOwned> &data);
    static Ref<Sound> Create(const MoteData &data);

  public:
    explicit Sound(SoundFileData &data);
    ~Sound();

    const SoundFileData &GetData() { return m_Data; }
    ma_audio_buffer *GetAudioBuffer();

  private:
    const SoundFileData m_Data;
    ma_audio_buffer_config m_AudioBufferConfig;
};
} // namespace Mana
