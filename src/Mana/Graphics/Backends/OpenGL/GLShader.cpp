#include <mpch.h>

#include "GLShader.h"

#include "Mana/Core/Log.h"
#include "Mana/Graphics/Backends/OpenGL/GL.h"
#include "Mana/Graphics/Shader.h"

namespace Mana {
GLType GLShader::ShaderTypeToOpenGL(ShaderType t) {
    switch (t) {
    case VERTEX_SHADER:
        return GL_VERTEX_SHADER;
    case FRAGMENT_SHADER:
        return GL_FRAGMENT_SHADER;
    case LAST:
        M_ASSERT(false, "Unkown shader type!")
        return GL_SHADER;
    }
}

GLShader::GLShader(const std::vector<CompiledShaderStage> &stages) {
    MC_TRACE("Loading shaders from SPIRV binaries");
    m_ProgramID = glCreateProgram();

    // Turn SPIRV into glsl shaders
    std::vector<GLShaderId> stageIds;
    for (const auto &stage : stages) {
        if (stage.CompiledFormat == SPIR_V) {
#if MANAB_OPENGL_45
            GLShaderId shaderId = stageIds.emplace_back(
                glCreateShader(ShaderTypeToOpenGL(stage.Type)));
            M_ASSERT(shaderId != 0,
                     "glCreateShader returned invalid shader id!");

            // Load Binary
            glShaderBinary(1, &shaderId, GL_SHADER_BINARY_FORMAT_SPIR_V,
                           stage.Data.data(),
                           stage.Data.size() * sizeof(uint8_t));

            glSpecializeShader(shaderId, "main", 0, nullptr, nullptr);

            MC_TRACE("Attaching Shader ({})",
                     ShaderUtils::ShaderTypeToShortString(stage.Type));

            int compiled = 0;
            glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compiled);
            if (compiled)
                glAttachShader(m_ProgramID, shaderId);
            else {
                MC_ERROR("Failed to load SPIRV binary!");
                // Get shader info log
                GLint infoLogLength = 0;
                glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
                if (infoLogLength > 0) {
                    std::vector<char> infoLog(infoLogLength);
                    glGetShaderInfoLog(shaderId, infoLogLength, nullptr,
                                       infoLog.data());
                    MC_ERROR("Shader info log: {}", infoLog.data());
                }
                return;
            }
#else
            MC_ERROR("Shader compiled to unsupported format! (SPIR_V but "
                     "OPENGL < 4.5)");
#endif
        } else if (stage.CompiledFormat == GLSLSource) {
            if (MANAB_OPENGL_45)
                MC_WARN("Using compiled shader format GLSLSource, but SPIR_V "
                        "is available");

            std::string sourceString(stage.Data.begin(), stage.Data.end());
            auto compiledShaderId = CompileShader(stage.Type, sourceString);

            if (compiledShaderId) {
                glAttachShader(m_ProgramID, compiledShaderId);
                stageIds.push_back(compiledShaderId);
            }
        } else {
            MC_ERROR("Unkown shader format!");
        }
    }

    // Link Program
    glLinkProgram(m_ProgramID);
    int success;
    glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_ProgramID, 512, NULL, infoLog);
        MC_ERROR("Failed to link shader, Error:\n{}", infoLog);
    }

    // Delete intermidietes
    for (auto id : stageIds) {
        glDeleteShader(id);
    }

    // Cache uniform locations
    GLint count;
    glGetProgramiv(m_ProgramID, GL_ACTIVE_UNIFORMS, &count);
    MC_TRACE("Program has {} uniforms", count);
    for (GLuint i = 0; i < count; i++) {
        const GLsizei bufSize = 16; // maximum name length
        GLchar name[bufSize];       // variable name in GLSL
        GLsizei length;             // name length
        GLint size;
        GLenum type;

        glGetActiveUniform(m_ProgramID, i, bufSize, &length, &size, &type,
                           name);

        MC_TRACE(" {}. {} (Size: {}, Type: {})", i, name, size, type);
        m_UniformLocations[std::string(name)] = i;

#if !MANAB_OPENGL_45
        if (type == GL_SAMPLER_2D) {
            MC_INFO("Binding samplers 0..{} to the uniform: {}", size - 1,
                    name);
            int samplers[size];
            for (int j = 0; j < size; j++) {
                samplers[j] = j;
            }

            Bind();
            glUniform1iv(i, size, samplers);
        }
#endif
    }
}

GLShader::GLShader(std::unordered_map<ShaderType, const std::string> srcs) {
    MC_TRACE("Compiling shaders for new shader program ({})", srcs.size());
    std::unordered_map<ShaderType, GLShaderId> compiled;
    for (auto &&[stage, src] : srcs) {
        compiled[stage] = CompileShader(stage, src);
    }

    m_ProgramID = glCreateProgram();

    // Attach all shaders
    for (auto &&[stage, id] : compiled) {
        MC_TRACE("Attaching Shader ({})",
                 ShaderUtils::ShaderTypeToShortString(stage));
        glAttachShader(m_ProgramID, id);
    }

    glLinkProgram(m_ProgramID);
    int success;
    glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_ProgramID, 512, NULL, infoLog);
        MC_ERROR("Failed to link shader, Error:\n{}", infoLog);
    }

    // Delete intermidietes
    for (auto &&[stage, id] : compiled) {
        glDeleteShader(id);
    }

    // Cache uniform locations
    GLint count;
    glGetProgramiv(m_ProgramID, GL_ACTIVE_UNIFORMS, &count);
    MC_TRACE("Program has {} uniforms", count);
    for (GLuint i = 0; i < count; i++) {
        const GLsizei bufSize = 16; // maximum name length
        GLchar name[bufSize];       // variable name in GLSL
        GLsizei length;             // name length
        GLint size;
        GLenum type;

        glGetActiveUniform(m_ProgramID, i, bufSize, &length, &size, &type,
                           name);

        MC_TRACE(" {}. {} (Size: {}, Type: {})", i, name, size, type);
        m_UniformLocations[std::string(name)] = i;

#if !MANAB_OPENGL_45
        if (type == GL_SAMPLER_2D) {
            MC_INFO("Binding samplers 0..{} to the uniform: {}", size - 1,
                    name);

            // TODO: GET MAX TEXTURE SUPPORT
            int samplers[size];
            for (int j = 0; j < size; j++) {
                samplers[j] = j;
            }

            Bind();
            glUniform1iv(i, size, samplers);
        }
#endif
    }
}

GLShader::~GLShader() {
    MC_TRACE("Deleting ShaderProgram");
    glDeleteProgram(m_ProgramID);
}

void GLShader::Bind() const { glUseProgram(m_ProgramID); }
void GLShader::Unbind() const { glUseProgram(0); }

GLShaderId GLShader::CompileShader(ShaderType type, const std::string &src) {
    MC_TRACE("Compiling Shader ({})",
             ShaderUtils::ShaderTypeToShortString(type));

    GLShaderId id = glCreateShader(ShaderTypeToOpenGL(type));
    const char *c_str = src.c_str();
    glShaderSource(id, 1, &c_str, NULL);

    // Compile
    int success;
    glCompileShader(id);
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        MC_ERROR("Failed to compile shader, Type: {}, Error:\n{}", (int)type,
                 infoLog);

        glDeleteShader(id);
        return 0;
    }

    return id;
}
void GLShader::SetFloat(const std::string &name, float value) {
    M_ASSERT(m_UniformLocations.contains(name));
    glUniform1f(m_UniformLocations[name], value);
}

void GLShader::SetFloat2(const std::string &name, const glm::vec2 &value) {
    M_ASSERT(m_UniformLocations.contains(name));
    glUniform2f(m_UniformLocations[name], value.x, value.y);
}

void GLShader::SetFloat4(const std::string &name, const glm::vec4 &value) {
    M_ASSERT(m_UniformLocations.contains(name));
    glUniform4f(m_UniformLocations[name], value.x, value.y, value.z, value.w);
}
void GLShader::SetMatrix(const std::string &name, const glm::mat4 &value) {
    glUniformMatrix4fv(m_UniformLocations[name], 1, GL_FALSE,
                       glm::value_ptr(value));
}

void GLShader::SetColor(const std::string &name, const Color &value) {
    M_ASSERT(m_UniformLocations.contains(name));
    glUniform4f(m_UniformLocations[name], value.r, value.g, value.b, value.a);
}
} // namespace Mana
