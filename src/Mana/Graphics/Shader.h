#pragma once
#include <filesystem>
#include <mpch.h>

#include "Mana/Core/Asset.h"

namespace Mana {
enum ShaderType {
    FRAGMENT_SHADER = 0,
    VERTEX_SHADER = 1,
    // GEOMETRY; COMPUTE
    LAST = 2,
};

enum CompiledShaderFormat {
    SPIR_V = 0,
    GLSLSource = 1,
};

struct CompiledShaderStage {
    const ShaderType Type;
    const CompiledShaderFormat CompiledFormat;
    const std::vector<uint8_t> Data;
};

// Packing function (assuming ShaderType needs 3 bits, FormatType needs 4 bits)
constexpr uint8_t PackShaderVersion(ShaderType shader,
                                    CompiledShaderFormat format) {
    return (static_cast<uint8_t>(shader) << 4) |
           (static_cast<uint8_t>(format) & 0x0F);
}

// Unpacking functions
constexpr ShaderType UnpackShaderType(uint8_t packed) {
    return static_cast<ShaderType>((packed >> 4) & 0x07); // Extract top 3 bits
}

constexpr CompiledShaderFormat UnpackShaderFormat(uint8_t packed) {
    return static_cast<CompiledShaderFormat>(packed &
                                             0x0F); // Extract bottom 4 bits
}

class ShaderUtils {
  public:
    static const char *ShaderTypeToShortString(ShaderType t);
    static ShaderType StringToShaderType(const std::string &s);
    static std::string ShaderTypeToString(const ShaderType t);
};

class Shader {
  public:
    static void CreateMote(std::filesystem::path src,
                           std::filesystem::path mote,
                           std::vector<MoteChunkOwned> &data);
    static Ref<Shader> Create(const MoteData &data);

  public:
    virtual ~Shader() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual void SetFloat(const std::string &name, float value) = 0;
    virtual void SetFloat2(const std::string &name, const glm::vec2 &value) = 0;
    virtual void SetFloat4(const std::string &name, const glm::vec4 &value) = 0;
    virtual void SetColor(const std::string &name, const Color &value) = 0;
    virtual void SetMatrix(const std::string &name, const glm::mat4 &value) = 0;
};

#ifdef M_HEADLESS
class NoShader final : public Shader {
  public:
    void Bind() const override {}
    void Unbind() const override {}

    void SetFloat4(const std::string &name, const glm::vec4 &value) override {}
    void SetMatrix(const std::string &name, const glm::mat4 &value) override {}
    void SetColor(const std::string &name, const Color &value) override {}
};
#endif
} // namespace Mana
