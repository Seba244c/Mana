#pragma once
#include <mpch.h>

#include "Mana/Graphics/GraphicsAPI.h"

namespace Mana {
struct GLInfo;
class GLAPI final : public GraphicsAPI {
  public:
    GLAPI(std::function<void()> swapWindowBuffersCallback,
          const GLInfo &glInfo);
    void Clear() override;
    void SetViewport(uint32_t x, uint32_t y, uint32_t width,
                     uint32_t height) override;
    void SetClearColor(const Color &clearColor) override;
    void SwapWindowDrawBuffers() override;
    void DrawIndexed(const Ref<VertexArray> &vao) override;
    void DrawIndexed(const Ref<VertexArray> &vao,
                     const uint32_t indexCount) override;
    void DrawArrays(const Ref<VertexArray> &vao, const uint32_t count) override;
    void DrawIndexedLines(const Ref<VertexArray> &vao,
                          const uint32_t indexCount) override;
    Ref<Shader>
    CreateShader(const std::vector<CompiledShaderStage> &stages) override;
    Ref<Texture> CreateTexture(const ImageData *data) override;
    Ref<VertexBuffer> CreateVertexBuffer(const uint32_t size) override;
    Ref<VertexBuffer> CreateVertexBuffer(const float *vertices,
                                         const uint32_t size) override;
    Ref<IndexBuffer> CreateIndexBuffer(const uint32_t *indices,
                                       uint32_t count) override;
    Ref<VertexArray> CreateVertexArray() override;
    Ref<Framebuffer> CreateFBO(const FramebufferSpecification &spec) override;
    int32_t GetMaxFragmentSamplers() override;

  private:
    std::function<void()> m_SwapWindowBuffersCallback;
    const GLInfo &m_GLInfo;
};
} // namespace Mana
