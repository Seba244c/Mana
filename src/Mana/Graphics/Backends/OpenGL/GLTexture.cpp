#include "mpch.h"

#include "GLTexture.h"

#include "Mana/Core/Log.h"
#include "Mana/Graphics/Texture.h"

namespace Mana {
static GLenum ImageFormatToGLDataFormat(const ImageFormat format) {
    switch (format) {
    case ImageFormat::RGB8:
        return GL_RGB;
    case ImageFormat::RGBA8:
        return GL_RGBA;
    case ImageFormat::R8:
        return GL_RED;
    case ImageFormat::RGBA32F:
        return GL_RGBA;
    case ImageFormat::None:
        return 0;
    }

    MC_ERROR("Unkown ImageFormat");
    return 0;
}

static GLenum ImageFormatToGLInternalFormat(const ImageFormat format) {
    switch (format) {
    case ImageFormat::RGB8:
        return GL_RGB8;
    case ImageFormat::RGBA8:
        return GL_RGBA8;
    case ImageFormat::R8:
        return GL_R8;
    case ImageFormat::RGBA32F:
        return GL_RGBA32F;
    case ImageFormat::None:
        return 0;
    }

    MC_ERROR("Unkown ImageFormat");
    return 0;
}

static GLint FilteringMethodToGLFilter(const FilteringMethod method) {
    switch (method) {
    case FilteringMethod::LINEAR:
        return GL_LINEAR;
    case FilteringMethod::NEAREST:
        return GL_NEAREST;
    case FilteringMethod::MIPMAP_CLOSEST_LINEAR:
        return GL_LINEAR_MIPMAP_NEAREST;
    case FilteringMethod::MIPMAP_CLOSEST_NEAREST:
        return GL_NEAREST_MIPMAP_NEAREST;
    case FilteringMethod::MIPMAP_INTERPOLATED_LINEAR:
        return GL_LINEAR_MIPMAP_LINEAR;
    case FilteringMethod::MIPMAP_INTERPOLATED_NEAREST:
        return GL_NEAREST_MIPMAP_LINEAR;
    case FilteringMethod::CLAMP_TO_EDGE:
        return GL_CLAMP_TO_EDGE;
    }

    MC_ERROR("Unkown FilteringMethod");
    return 0;
}

GLTexture::GLTexture(const ImageData *data) {
    MC_TRACE("Creating GL Texture");
    m_Width = data->Width;
    m_Height = data->Height;
    ImageFormat format = GetImageFormat(data);

    // Create Texture
    glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
    glTextureStorage2D(m_RendererID, 1, ImageFormatToGLInternalFormat(format),
                       m_Width, m_Height);

    glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER,
                        FilteringMethodToGLFilter(data->minifyFilter));
    glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER,
                        FilteringMethodToGLFilter(data->minifyFilter));
    glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Upload data
    M_ASSERT(data->Data.size() == m_Width * m_Height * data->Channels);
    if (data->Channels == 4)
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // The default
    else
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height,
                        ImageFormatToGLDataFormat(format), GL_UNSIGNED_BYTE,
                        data->Data.data());
}

GLTexture::GLTexture(uint32_t width, uint32_t height, GLenum internalFormat,
                     FilteringMethod mag, FilteringMethod min) {
    MC_TRACE("Creating GL Texture");
    m_Width = width;
    m_Height = height;
    glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
    glTexStorage2D(GL_TEXTURE_2D, 1, internalFormat, width, height);
    glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER,
                        FilteringMethodToGLFilter(min));
    glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER,
                        FilteringMethodToGLFilter(mag));
    glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
void GLTexture::Bind(const uint32_t slot) const {
    glBindTextureUnit(slot, m_RendererID);
}

GLTexture::~GLTexture() { glDeleteTextures(1, &m_RendererID); }
} // namespace Mana
