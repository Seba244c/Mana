#include "Sound.h"

namespace Mana {
void Sound::CreateMote(std::filesystem::path src,
                       std::vector<MoteChunkOwned> &data) {
    if (src.extension() == ".ogg") {
        MC_WARN(".ogg support is waiting for miniaudio 1.12");
        return;
    }
    MC_TRACE("Creating sound mote");
    // Get data
    auto raw = Utils::Files::loadBinaryFromFile(src);

    // Decode Data
    ma_decoder decoder;
    ma_result result = ma_decoder_init_file(src.c_str(), NULL, &decoder);
#ifdef M_DEBUG_INFO
    if (result != MA_SUCCESS) {
        MC_ERROR("ma_decoder_file_init({0}, ..) returned {1}", src.c_str(),
                 (int)result);
        if (result == MA_INVALID_FILE)
            MC_ERROR("MA_INVALID_FILE, Path: {0}", src.c_str());
    }
#endif
    M_ASSERT(result == MA_SUCCESS, "Failed to initialize audio file decoder");

    // Get audio properties
    ma_uint64 framesCount = 0;
    uint32_t channels = decoder.outputChannels;
    uint32_t sampleRate = decoder.outputSampleRate;
    ma_format format = decoder.outputFormat;

    // Get frame count
    ma_decoder_get_length_in_pcm_frames(&decoder, &framesCount);
    MC_TRACE("Soundfile decoded with: {} channels and {} frames at {}Hz",
             channels, framesCount, sampleRate);

    // Read PCM frames
    ma_uint32 bytesPerSample = ma_get_bytes_per_sample(format);
    ma_uint32 bytesPerFrame = bytesPerSample * channels;
    const size_t totalBytes = framesCount * bytesPerFrame;

    std::vector<uint8_t> pcmData(totalBytes);
    ma_uint64 framesRead;
    result = ma_decoder_read_pcm_frames(&decoder, pcmData.data(), framesCount,
                                        &framesRead);
    M_ASSERT(result == MA_SUCCESS && framesRead == framesCount);
    M_ASSERT(pcmData.size() == totalBytes,
             "Unable to allocate memory for PCM data!");

    // Write as pcm
    data.push_back(Motes::StructToChunk<SoundMetadataChunk>(
        0, {framesCount, channels, sampleRate, format, RawPCM}));
    data.push_back(Motes::CompressDataToChunk(1, pcmData));

    // Free decoder
    ma_decoder_uninit(&decoder);
}

Ref<Sound> Sound::Create(const MoteData &data) {
    SoundFileData output;

    for (auto c : data.Chunks) {
        if (c.Header->Type == 0) {
            auto metadata = Motes::ChunkToStruct<SoundMetadataChunk>(c);
            output.MetaData.Channels = metadata->Channels;
            output.MetaData.FrameCount = metadata->FrameCount;
            output.MetaData.SampleRate = metadata->SampleRate;
            output.MetaData.DataFormat = metadata->DataFormat;
            output.MetaData.CompressionFormat = metadata->CompressionFormat;
        } else if (c.Header->Type == 1) {
            output.Data = Motes::ReadCompressedBinaryData(c);
        }
    }
    MC_TRACE(
        "Creating Sound assset that with: {} channels and {} frames at {}Hz",
        output.MetaData.Channels, output.MetaData.FrameCount,
        output.MetaData.SampleRate);

    return CreateRef<Sound>(output);
}

Sound::Sound(SoundFileData &data) : m_Data(std::move(data)) {
    M_ASSERT(m_Data.MetaData.Channels > 0,
             "Soundfile must have at least 1 channel!");
    M_ASSERT(m_Data.Data.size() ==
                 m_Data.MetaData.FrameCount * m_Data.MetaData.Channels *
                     ma_get_bytes_per_sample(m_Data.MetaData.DataFormat),
             "Insufficient PCM Data");
    m_AudioBufferConfig = ma_audio_buffer_config_init(
        m_Data.MetaData.DataFormat, m_Data.MetaData.Channels,
        m_Data.MetaData.FrameCount, m_Data.Data.begin().base(), NULL);
    m_AudioBufferConfig.sampleRate = m_Data.MetaData.SampleRate;
}

Sound::~Sound() { MC_TRACE("Sound was destructed!"); }

ma_audio_buffer *Sound::GetAudioBuffer() {
    ma_audio_buffer *audioBuffer = new ma_audio_buffer;
    if (ma_audio_buffer_init(&m_AudioBufferConfig, audioBuffer) != MA_SUCCESS) {
        MC_WARN("Failed to initialize audio buffer! Sound initialization will "
                "fail");
        ma_audio_buffer_uninit(audioBuffer);
        return nullptr;
    }
    return audioBuffer;
}
} // namespace Mana
