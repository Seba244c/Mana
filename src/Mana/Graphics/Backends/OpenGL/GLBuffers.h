#pragma once
#include "Mana/Graphics/Buffers.h"

typedef GLType GLVertexBufferID;
typedef GLType GLIndexBufferID;

namespace Mana {
class GLVertexBuffer final : public VertexBuffer {
  public:
    GLVertexBuffer(const float *vertices, uint32_t size);
    GLVertexBuffer(uint32_t size);
    ~GLVertexBuffer() override;

    [[nodiscard]] const BufferLayout &GetLayout() const override {
        return m_Layout;
    }
    void SetLayout(const BufferLayout &layout) override { m_Layout = layout; }

    void SetData(const void *data, uint32_t size) override;

    void Bind() const override;
    void Unbind() const override;

  private:
    GLVertexBufferID m_RendererID;
    BufferLayout m_Layout;
};

class GLIndexBuffer final : public IndexBuffer {
  public:
    GLIndexBuffer(const uint32_t *indices, uint32_t count);
    ~GLIndexBuffer() override;

    void Bind() const override;
    void Unbind() const override;

    [[nodiscard]] uint32_t GetCount() const override { return m_Count; }

  private:
    GLIndexBufferID m_RendererID;
    uint32_t m_Count;
};
} // namespace Mana
