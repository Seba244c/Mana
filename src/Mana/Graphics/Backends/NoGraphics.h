#pragma once
#include "mpch.h"

#ifdef M_HEADLESS
#include "Mana/Graphics/GraphicsAPI.h"
#include "Mana/Graphics/GraphicsContext.h"
#include "Mana/IO/Window.h"

namespace Mana {
class NoGraphicsAPI final : public GraphicsAPI {
  public:
    NoGraphicsAPI() {}
    void Clear() override {}
    void SetViewport(uint32_t x, uint32_t y, uint32_t width,
                     uint32_t height) override {}
    void SetClearColor(const Color &clearColor) override {}
    void SwapWindowDrawBuffers() override {}
    void DrawIndexed(const Ref<VertexArray> &vao) override {}
    void DrawIndexed(const Ref<VertexArray> &vao,
                     const uint32_t indexCount) override {}
    void DrawIndexedLines(const Ref<VertexArray> &vao,
                          const uint32_t indexCount) override {}
    Ref<Shader>
    CreateShader(const std::vector<CompiledShaderStage> &stages) override {
        return CreateRef<NoShader>();
    }
    Ref<Texture> CreateTexture(const ImageFileData *data) override {
        return CreateRef<NoTexture>();
    }
    Ref<VertexBuffer> CreateVertexBuffer(const uint32_t size) override {
        return CreateRef<NoVertexBuffer>();
    }
    Ref<VertexBuffer> CreateVertexBuffer(const float *vertices,
                                         const uint32_t size) override {
        return CreateRef<NoVertexBuffer>();
    }
    Ref<IndexBuffer> CreateIndexBuffer(const uint32_t *indices,
                                       uint32_t count) override {
        return CreateRef<NoIndexBuffer>();
    }
    Ref<VertexArray> CreateVertexArray() override {
        return CreateScope<NoVertexArray>();
    }
    int32_t GetMaxFragmentSamplers() override { return 16; }
    float GetMaxLinesWidth() override { return 1; }
};

class NoGraphics : public GraphicsContext {
  public:
    NoGraphics(Window &window) {}

    Scope<GraphicsAPI> GetAPI() override {
        return CreateScope<NoGraphicsAPI>();
    }
};
} // namespace Mana
#endif
