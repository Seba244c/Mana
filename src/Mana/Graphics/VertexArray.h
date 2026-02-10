#pragma once
#include "Buffers.h"

namespace Mana {
class VertexArray {
  public:
    virtual ~VertexArray() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual void AddVertexBuffer(const Ref<VertexBuffer> &vertexBuffer) = 0;
    virtual void SetIndexBuffer(const Ref<IndexBuffer> &indexBuffer) = 0;

    [[nodiscard]] virtual const std::vector<Ref<VertexBuffer>> &
    GetVertexBuffers() const = 0;
    [[nodiscard]] virtual const Ref<IndexBuffer> &GetIndexBuffer() const = 0;
};

#ifdef M_HEADLESS
class NoVertexArray final : public VertexArray {
    void Bind() const override {}
    void Unbind() const override {}

    void AddVertexBuffer(const Ref<VertexBuffer> &vertexBuffer) override {}
    void SetIndexBuffer(const Ref<IndexBuffer> &indexBuffer) override {}

    [[nodiscard]] const std::vector<Ref<VertexBuffer>> &
    GetVertexBuffers() const override {
        return {};
    }
    [[nodiscard]] const Ref<IndexBuffer> &GetIndexBuffer() const override {
        return nullptr;
    }
};
#endif
} // namespace Mana
