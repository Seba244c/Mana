#include "GLVertexArray.h"

namespace Mana {
static GLenum ShaderDataTypeToGLBaseType(ShaderDataType type) {
    switch (type) {
    case ShaderDataType::None:
        return GL_FLOAT;
    case ShaderDataType::Float:
        return GL_FLOAT;
    case ShaderDataType::Float2:
        return GL_FLOAT;
    case ShaderDataType::Float3:
        return GL_FLOAT;
    case ShaderDataType::Float4:
        return GL_FLOAT;
    case ShaderDataType::Mat3:
        return GL_FLOAT;
    case ShaderDataType::Mat4:
        return GL_FLOAT;
    case ShaderDataType::UInt:
        return GL_UNSIGNED_INT;
    case ShaderDataType::Int2:
        return GL_INT;
    case ShaderDataType::Int3:
        return GL_INT;
    case ShaderDataType::Int4:
        return GL_INT;
    case ShaderDataType::Bool:
        return GL_BOOL;
    }

    MC_ERROR("Unknown ShaderDataType!");
    return 0;
}

GLVertexArray::GLVertexArray() { glCreateVertexArrays(1, &m_RendererID); }

GLVertexArray::~GLVertexArray() { glDeleteVertexArrays(1, &m_RendererID); }

void GLVertexArray::Bind() const { glBindVertexArray(m_RendererID); }

void GLVertexArray::Unbind() const { glBindVertexArray(0); }

void GLVertexArray::AddVertexBuffer(const Ref<VertexBuffer> &vertexBuffer) {
    glBindVertexArray(m_RendererID);
    vertexBuffer->Bind();

    uint32_t i = 0;
    const auto &layout = vertexBuffer->GetLayout();
    for (const auto &element : layout) {
        glEnableVertexAttribArray(i);
        if (element.Type == ShaderDataType::UInt) {
            glVertexAttribIPointer(
                i, element.GetComponentCount(), GL_UNSIGNED_INT,
                layout.GetStride(),
                reinterpret_cast<const void *>(element.Offset));

        } else {
            glVertexAttribPointer(
                i, element.GetComponentCount(),
                ShaderDataTypeToGLBaseType(element.Type),
                element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(),
                reinterpret_cast<const void *>(element.Offset));
        }
        i++;
    }

    m_VertexBuffers.push_back(vertexBuffer);
}

void GLVertexArray::SetIndexBuffer(const Ref<IndexBuffer> &indexBuffer) {
    glBindVertexArray(m_RendererID);
    indexBuffer->Bind();

    m_IndexBuffer = indexBuffer;
}

} // namespace Mana
