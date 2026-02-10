#include "GL.h"

#ifndef GL_VERSION_4_5
// Emulate glCreateTextures using OpenGL 4.1 features
void glCreateTextures(GLenum target, GLsizei n, GLuint *textures) {
    glGenTextures(n, textures);

    for (GLsizei i = 0; i < n; ++i) {
        glBindTexture(target, textures[i]);
    }
}

// Emulate glCreateBuffers using OpenGL 4.1 features
void glCreateBuffers(GLsizei n, GLuint *buffers) {
    glGenBuffers(n, buffers);

    for (GLsizei i = 0; i < n; ++i) {
        glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
    }
}

void glCreateVertexArrays(GLsizei n, GLuint *arrays) {
    glGenVertexArrays(n, arrays);

    for (GLsizei i = 0; i < n; ++i) {
        glBindVertexArray(arrays[i]);
    }
}

// emulate glTextureParameteri using OpenGL 4.1 features
void glTextureParameteri(GLuint texture, GLenum pname, GLenum param) {
    GLint prev;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prev);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, pname, param);

    glBindTexture(GL_TEXTURE_2D, prev);
}

void glTextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat,
                        GLsizei width, GLsizei height) {
    GLint prev;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prev);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Max Levels
    GLsizei maxLevels = 1 + std::floor(std::log2(std::max(width, height)));
    levels = std::min(levels, maxLevels);

    // We might still be able to use glTexStorage2D if the ARB is enabled
#ifdef GLAD_ARB_texture_storage
    glTexStorage2D(GL_TEXTURE_2D, levels, internalformat, width, height);
#else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, levels - 1);
    GLsizei w = width;
    GLsizei h = height;

    for (GLsizei level = 0; level < levels; ++level) {
        GLenum format, type;
        switch (internalformat) {
        case GL_RGB8:
            format = GL_RGB;
            type = GL_UNSIGNED_BYTE;
            break;
        case GL_RGBA8:
            format = GL_RGBA;
            type = GL_UNSIGNED_BYTE;
            break;
        case GL_R8:
            format = GL_RED;
            type = GL_UNSIGNED_BYTE;
            break;
        case GL_RG8:
            format = GL_RG;
            type = GL_UNSIGNED_BYTE;
            break;
        default:
            format =
                GL_RGBA; // Fallback (may still be incorrect for some formats)
            type = GL_UNSIGNED_BYTE;
            break;
        }

        glTexImage2D(GL_TEXTURE_2D, level, internalformat, w, h, 0, format,
                     type, nullptr);

        // Halve the dimensions for the next mimap level
        w = std::max(1, w / 2);
        h = std::max(1, h / 2);
    }
#endif

    glBindTexture(GL_TEXTURE_2D, prev);
}

void glTextureSubImage2D(GLuint texture, GLint level, GLint xoffset,
                         GLint yoffset, GLsizei width, GLsizei height,
                         GLenum format, GLenum type, const void *pixels) {
    GLint prev;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prev);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexSubImage2D(GL_TEXTURE_2D, level, xoffset, yoffset, width, height,
                    format, type, pixels);

    glBindTexture(GL_TEXTURE_2D, prev);
}

void glBindTextureUnit(GLuint unit, GLuint texture) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);
}
#endif
