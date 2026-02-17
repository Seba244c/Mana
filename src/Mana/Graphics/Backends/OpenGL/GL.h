#pragma once

#include <glad/gl.h>
#undef GLAPI
typedef uint32_t GLType;

#ifndef GL_VERSION_4_5
// Emulate 4.5+ exclusive functions, using OpenGL 4.1 features
void glCreateTextures(GLenum target, GLsizei n, GLuint *textures);
void glCreateBuffers(GLsizei n, GLuint *buffers);
void glCreateFramebuffers(GLsizei n, GLuint *framebuffers);
void glCreateVertexArrays(GLsizei n, GLuint *arrays);
void glTextureParameteri(GLuint texture, GLenum pname, GLenum param);
void glTextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat,
                        GLsizei width, GLsizei height);
void glTexStorage2D(GLuint texture, GLsizei levels, GLenum internalformat,
                    GLsizei width, GLsizei height);
void glTextureSubImage2D(GLuint texture, GLint level, GLint xoffset,
                         GLint yoffset, GLsizei width, GLsizei height,
                         GLenum format, GLenum type, const void *pixels);
void glBindTextureUnit(GLuint unit, GLuint texture);

#ifndef GL_SHADER
#define GL_SHADER GL_VERTEX_SHADER
#endif
#define MANAB_OPENGL_45 0
#define MANAB_OPENGL_MAX_TEXTURE 1
#else
#define MANAB_OPENGL_45 1
#define MANAB_OPENGL_MAX_TEXTURE 32
#endif
