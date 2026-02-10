#include "Shader.h"
#include "Mana/Core/Application.h"
#include "Mana/Core/Asset.h"
#include "Mana/Core/Core.h"
#include "Mana/Core/Log.h"
#include "Mana/Core/Utils.h"
#include <fstream>

namespace Mana {
const char *ShaderUtils::ShaderTypeToShortString(ShaderType t) {
    switch (t) {
    case VERTEX_SHADER:
        return "VERT";
    case FRAGMENT_SHADER:
        return "FRAG";
    case LAST:
        return "";
    }

    return "";
}
ShaderType ShaderUtils::StringToShaderType(const std::string &s) {
    if (s == "vertex")
        return VERTEX_SHADER;
    if (s == "fragment")
        return FRAGMENT_SHADER;

    return LAST;
}

std::string ShaderUtils::ShaderTypeToString(ShaderType t) {
    if (t == VERTEX_SHADER)
        return "vertex";
    if (t == FRAGMENT_SHADER)
        return "fragment";

    return "";
}

CompiledShaderStage ReadShaderDataFromTempFile(ShaderType type,
                                               std::string path) {
    return {type, SPIR_V, Utils::Files::loadBinaryFromFile(path)};
}

void WriteShaderToMoteFile(std::vector<MoteChunkOwned> &data,
                           const std::vector<CompiledShaderStage> &d) {
    for (const auto &stage : d) {
        data.push_back(Motes::CompressDataToChunk(
            PackShaderVersion(stage.Type, stage.CompiledFormat), stage.Data));
    }
}

Ref<Shader> Shader::Create(const MoteData &data) {
    // Get amount of stages
    MC_TRACE("Loading shader from mote with {} stages", data.Chunks.size());

    // Read Each Stage
    std::vector<CompiledShaderStage> stages;
    for (auto chunk : data.Chunks) {
        auto type = UnpackShaderType(chunk.Header->Type);
        auto format = UnpackShaderFormat(chunk.Header->Type);
        stages.push_back(CompiledShaderStage{
            type, format, Motes::ReadCompressedBinaryData(chunk)});
    }

    return Application::Instance()->GetGraphicsAPI().CreateShader(stages);
}

std::vector<uint8_t>
CreateStageSourceWithInjections(std::filesystem::path src, std::string version,
                                std::string type,
                                std::vector<std::string> macros) {
    MC_TRACE("Injeting version {}, type {} and {} macros", version, type,
             macros.size());
    auto sourceBinary = Utils::Files::loadBinaryFromFile(src);
    std::string prefix;
    prefix += "#version " + version + "\n";
    prefix += "#define " + type + "\n";
    prefix +=
        "#define MAX_TEXTURES " MACRO_TOSTRING(MANAB_OPENGL_MAX_TEXTURE) "\n";

    prefix += "#if defined(vertex)\n"
              "#define varying(l, t, n, s) layout(location = l) s out t n;\n"
              "#else\n"
              "#define varying(l, t, n, s) layout(location = l) s in t n;\n"
              "#endif\n";

    for (auto &macro : macros) {
        if (!macro.starts_with('#'))
            prefix += "#define " + macro + "\n";
        else
            prefix += macro + "\n";
    }

    sourceBinary.insert(sourceBinary.begin(), prefix.begin(), prefix.end());
    return sourceBinary;
}

void Shader::CreateMote(std::filesystem::path src, std::filesystem::path mote,
                        std::vector<MoteChunkOwned> &data) {

    // Get list of stages
    std::vector<std::string> shaderStages;
    std::ifstream in(src);

    std::string line;
    getline(in, line);

    auto types = Utils::String::splitString(line, ',');
    types.erase(types.begin());
    for (auto type : types)
        shaderStages.push_back(type);
    in.close();

    // Compile and store stages
    std::vector<CompiledShaderStage> stages;
    for (auto stage : shaderStages) {
#if MANAB_OPENGL_45
        MC_TRACE("Comiling shader stage to SPIR_V with glslc: {}", stage);

        // Create IM file with injections
        auto glslSource = CreateStageSourceWithInjections(
            src, "450 core", stage,
            {"uniform(l, t, n) layout ( location = l ) uniform t n;",
             "uniformTexture(l, n) layout ( binding = l ) uniform sampler2D "
             "n;"});
        Utils::Files::TempFile IMFile((mote.string() + "." + stage + ".im"),
                                      glslSource);
        Utils::Files::TempFile OutFile((mote.string() + "." + stage));

        // Prepare command
        auto command = std::format(
            "glslc --target-env=opengl4.5  -fshader-stage={} {} -o {}", stage,
            IMFile.Path().string(), OutFile.Path().string());
        MC_TRACE("Building shader with command: {}", command);

        // Check if glslc is available
        int result = system("glslc --version");
        if (result != 0) {
            MC_ERROR("glslc, not available. Cannot build shader");
            return;
        }

        // Build shader
        result = system(command.c_str());
        if (result != 0) {
            MC_ERROR("glslc failed with output: {}", result);
        }

        // Read file to memory again, and remove temp file
        stages.push_back(ReadShaderDataFromTempFile(
            ShaderUtils::StringToShaderType(stage), OutFile.Path()));
#else
        MC_TRACE("Shipping shader stage as raw GLSL: {}", stage);
        auto shaderType = ShaderUtils::StringToShaderType(stage);

        // We want to inject definitons into the compiler so it understands
        // wether it's a vertex and such akin to -Dvertex=1 but the lamer
        // variant Also inject the version
        auto glslSource = CreateStageSourceWithInjections(
            src, "410 core", stage,
            {"uniform(l, t, n) uniform t n;",
             "uniformTexture(l, n) uniform sampler2D n;"});
        stages.emplace_back(shaderType, GLSLSource, glslSource);
#endif
    }

    WriteShaderToMoteFile(data, stages);
}
} // namespace Mana
