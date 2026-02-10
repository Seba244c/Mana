#pragma once
#include "mpch.h"

#include "Mana/Graphics/VertexArray.h"

typedef GLType GLVertexArrayID;

namespace Mana {
class GLVertexArray final : public VertexArray {
  public:
    GLVertexArray();
    ~GLVertexArray() override;

    void Bind() const override;
    void Unbind() const override;

    void AddVertexBuffer(const Ref<VertexBuffer> &vertexBuffer) override;
    void SetIndexBuffer(const Ref<IndexBuffer> &indexBuffer) override;

    [[nodiscard]] const std::vector<Ref<VertexBuffer>> &
    GetVertexBuffers() const override {
        return m_VertexBuffers;
    }
    [[nodiscard]] const Ref<IndexBuffer> &GetIndexBuffer() const override {
        return m_IndexBuffer;
    }

  private:
    GLVertexArrayID m_RendererID;
    std::vector<Ref<VertexBuffer>> m_VertexBuffers;
    Ref<IndexBuffer> m_IndexBuffer;
};
} // namespace Mana
