#include "Mana/Core/Application.h"
#include "Mana/Core/Asset.h"
#include "Mana/Core/Utils.h"

#include "Texture.h"
#if M_DEBUG
#include "stb_image.h"
#endif

namespace Mana {
void Texture::CreateMote(std::filesystem::path src,
                         std::vector<MoteChunkOwned> &data) {
#if M_DEBUG
    MC_TRACE("Creating image mote");
    // Get data
    auto raw = Utils::Files::loadBinaryFromFile(src);

    // Convert Data
    ImageData image;
    stbi_set_flip_vertically_on_load(1);
    auto imageData = stbi_load_from_memory(raw.data(), raw.size(), &image.Width,
                                           &image.Height, &image.Channels, 0);
    MC_TRACE("Image is {}x{} and has {} channels", image.Width, image.Height,
             image.Channels);

    // Put it into the vector temporarily, so we don't have to copy the data
    image.Data.assign(
        imageData, imageData + (image.Width * image.Height * image.Channels));

    data.push_back(Motes::StructToChunk<ImageMetadataChunk>(
        0, {image.Width, image.Height, image.Channels}));
    data.push_back(Motes::CompressDataToChunk(1, image.Data));

    // Free memory
    stbi_image_free(imageData);
#else
    MC_ERROR(
        "Tried to compile texture in release mode. This is not supported!");
#endif
}

Ref<Texture> Texture::Create(const MoteData &data) {
    ImageData image;
    for (auto c : data.Chunks) {
        if (c.Header->Type == 0) {
            auto MetaData = Motes::ChunkToStruct<ImageMetadataChunk>(c);
            image.Width = MetaData->Width;
            image.Height = MetaData->Height;
            image.Channels = MetaData->Channels;
        } else if (c.Header->Type == 1) {
            image.Data = Motes::ReadCompressedBinaryData(c);
        }
    }
    MC_TRACE("Creaing image that is {}x{} and has {} channels", image.Width,
             image.Height, image.Channels);
    return Application::Instance()->GetGraphicsAPI().CreateTexture(&image);
}

ImageFormat GetImageFormat(const ImageData *data) {
    return ImageFormat(data->Channels);
}
} // namespace Mana
