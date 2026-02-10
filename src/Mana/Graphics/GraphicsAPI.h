#pragma once
#include "mpch.h"

#include "Mana/Graphics/Framebuffer.h"
#include "Mana/Graphics/Shader.h"
#include "Mana/Graphics/Texture.h"
#include "Mana/Graphics/VertexArray.h"

namespace Mana {
class GraphicsAPI {
  public:
    virtual ~GraphicsAPI() = default;
    virtual void Clear() = 0;

    virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width,
                             uint32_t height) = 0;
    virtual void SetClearColor(const Color &clearColor) = 0;
    virtual void SwapWindowDrawBuffers() = 0;

    virtual void DrawIndexed(const Ref<VertexArray> &vao) = 0;
    virtual void DrawIndexed(const Ref<VertexArray> &vao,
                             const uint32_t indexCount) = 0;
    virtual void DrawArrays(const Ref<VertexArray> &vao,
                            const uint32_t count) = 0;
    virtual void DrawIndexedLines(const Ref<VertexArray> &vao,
                                  const uint32_t indexCount) = 0;
    virtual Ref<Shader>
    CreateShader(const std::vector<CompiledShaderStage> &stages) = 0;
    virtual Ref<Texture> CreateTexture(const ImageData *data) = 0;

    virtual Ref<VertexBuffer> CreateVertexBuffer(const uint32_t size) = 0;
    virtual Ref<VertexBuffer> CreateVertexBuffer(const float *vertices,
                                                 const uint32_t size) = 0;
    virtual Ref<IndexBuffer> CreateIndexBuffer(const uint32_t *indices,
                                               uint32_t count) = 0;
    virtual Ref<VertexArray> CreateVertexArray() = 0;
    virtual Ref<Framebuffer>
    CreateFBO(const FramebufferSpecification &spec) = 0;
    virtual int32_t GetMaxFragmentSamplers() = 0;
};
} // namespace Mana
