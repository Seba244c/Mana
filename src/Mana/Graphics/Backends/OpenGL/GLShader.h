#pragma once
#include <mpch.h>

#include "Mana/Graphics/Shader.h"

typedef GLType GLShaderId;
typedef GLType GLProgramId;

namespace Mana {
class GLShader final : public Shader {
  public:
    static GLType ShaderTypeToOpenGL(ShaderType t);

  public:
    GLShader(std::unordered_map<ShaderType, const std::string> srcs);
    GLShader(const std::vector<CompiledShaderStage> &stages);
    ~GLShader() override;

    void Bind() const override;
    void Unbind() const override;

    void SetFloat(const std::string &name, float value) override;
    void SetFloat2(const std::string &name, const glm::vec2 &value) override;
    void SetFloat4(const std::string &name, const glm::vec4 &value) override;
    void SetMatrix(const std::string &name, const glm::mat4 &value) override;
    void SetColor(const std::string &name, const Color &value) override;

  private:
    GLShaderId CompileShader(ShaderType type, const std::string &src);

  private:
    GLProgramId m_ProgramID;
    std::unordered_map<std::string, GLint> m_UniformLocations;
};
} // namespace Mana
