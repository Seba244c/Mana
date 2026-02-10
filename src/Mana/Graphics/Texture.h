#pragma once

#include "Mana/Core/Asset.h"
namespace Mana {

struct ImageMetadataChunk {
    int Width, Height, Channels;
};

enum class FilteringMethod {
    LINEAR = 0,
    NEAREST,

    MIPMAP_CLOSEST_LINEAR, //  takes the nearest mipmap level and samples that
                           //  level using linear interpolation.
                           //  (GL_LINEAR_MIPMAP_NEAREST)

    MIPMAP_CLOSEST_NEAREST, //  takes the nearest mipmap to match the pixel size
                            //  and (GL_NEAREST_MIPMAP_NEAREST)

    MIPMAP_INTERPOLATED_LINEAR, //  linearly interpolates between the two
                                //  closest mipmaps and samples the interpolated
                                //  level via linear interpolation.
                                //  (GL_LINEAR_MIPMAP_LINEAR)

    MIPMAP_INTERPOLATED_NEAREST, //  linearly interpolates between the two
                                 //  mipmaps that most closely match the size of
                                 //  a pixel and samples the interpolated level
                                 //  via nearest neighbor interpolation.
                                 //  (GL_NEAREST_MIPMAP_LINEAR)
    CLAMP_TO_EDGE,
};

struct ImageData {
    int Width, Height, Channels;
    std::vector<uint8_t> Data;

    FilteringMethod minifyFilter = FilteringMethod::NEAREST;
    FilteringMethod magnifyFilter = FilteringMethod::NEAREST;
};

enum class ImageFormat { None = 0, R8 = 1, RGB8 = 3, RGBA8 = 4, RGBA32F };
ImageFormat GetImageFormat(const ImageData *data);

class Texture {
  public:
    static void CreateMote(std::filesystem::path src,
                           std::vector<MoteChunkOwned> &data);
    static Ref<Texture> Create(const MoteData &data);

  public:
    [[nodiscard]] virtual uint32_t GetWidth() const = 0;
    [[nodiscard]] virtual uint32_t GetHeight() const = 0;
    [[nodiscard]] virtual uint32_t GetRendererID() const = 0;
    virtual void Bind(uint32_t slot = 0) const = 0;

    virtual ~Texture() = default;
};

#ifdef M_HEADLESS
class NoTexture final : public Texture {
  public:
    [[nodiscard]] uint32_t GetWidth() const override { return 1; }
    [[nodiscard]] uint32_t GetHeight() const override { return 1; }
    [[nodiscard]] uint32_t GetRendererID() const override { return 0; }
    void Bind(uint32_t slot) const override {}
};
#endif
} // namespace Mana
