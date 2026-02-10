#include "mpch.h"

#include "GLAPI.h"
#include "Mana/Graphics/Backends/OpenGL/GLBuffers.h"
#include "Mana/Graphics/Backends/OpenGL/GLContext.h"
#include "Mana/Graphics/Backends/OpenGL/GLFramebuffer.h"
#include "Mana/Graphics/Backends/OpenGL/GLShader.h"
#include "Mana/Graphics/Backends/OpenGL/GLTexture.h"
#include "Mana/Graphics/Backends/OpenGL/GLVertexArray.h"
#include "Mana/Graphics/Buffers.h"
#include "Mana/Graphics/Shader.h"
#include "Mana/Graphics/VertexArray.h"

namespace Mana {
GLAPI::GLAPI(std::function<void()> swapWindowBuffersCallback,
             const GLInfo &glInfo)
    : m_GLInfo(glInfo) {
    m_SwapWindowBuffersCallback = swapWindowBuffersCallback;
}

void GLAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width,
                        uint32_t height) {
    glViewport(x, y, width, height);
}

void GLAPI::Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }
void GLAPI::SetClearColor(const Color &clearColor) {
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
}
void GLAPI::SwapWindowDrawBuffers() { m_SwapWindowBuffersCallback(); }
void GLAPI::DrawIndexed(const Ref<VertexArray> &vao) {
    vao->Bind();

    glDrawElements(GL_TRIANGLES, vao->GetIndexBuffer()->GetCount(),
                   GL_UNSIGNED_INT, nullptr);
}

void GLAPI::DrawIndexed(const Ref<VertexArray> &vao,
                        const uint32_t indexCount) {
    vao->Bind();

    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}

void GLAPI::DrawArrays(const Ref<VertexArray> &vao, const uint32_t count) {
    vao->Bind();

    glDrawArrays(GL_TRIANGLES, 0, count);
}

void GLAPI::DrawIndexedLines(const Ref<VertexArray> &vao,
                             const uint32_t indexCount) {
    vao->Bind();

    glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_INT, nullptr);
}

Ref<Shader>
GLAPI::CreateShader(const std::vector<CompiledShaderStage> &stages) {
    return CreateRef<GLShader>(stages);
}
Ref<Texture> GLAPI::CreateTexture(const ImageData *data) {
    return CreateRef<GLTexture>(data);
}
Ref<VertexBuffer> GLAPI::CreateVertexBuffer(const uint32_t size) {
    return CreateRef<GLVertexBuffer>(size);
}
Ref<VertexBuffer> GLAPI::CreateVertexBuffer(const float *vertices,
                                            const uint32_t size) {
    return CreateRef<GLVertexBuffer>(vertices, size);
}
Ref<IndexBuffer> GLAPI::CreateIndexBuffer(const uint32_t *indices,
                                          uint32_t count) {
    return CreateRef<GLIndexBuffer>(indices, count);
}
Ref<VertexArray> GLAPI::CreateVertexArray() {
    return CreateRef<GLVertexArray>();
}
Ref<Framebuffer> GLAPI::CreateFBO(const FramebufferSpecification &spec) {
    return CreateRef<GLFramebuffer>(spec);
}

int32_t GLAPI::GetMaxFragmentSamplers() { return m_GLInfo.MaxFragmentSamplers; }
} // namespace Mana
