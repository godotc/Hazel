//
// Created by nono on 9/20/23.
//

#include "hz_pch.h"

#include "glad/glad.h"
#include "opengl_vertex_array.h"
#include "hazel/renderer/buffer.h"

namespace hazel {

// clang-format off
static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
    switch (type) {
        case ShaderDataType::Float:  return GL_FLOAT;
        case ShaderDataType::Float2: return GL_FLOAT;
        case ShaderDataType::Float3: return GL_FLOAT;
        case ShaderDataType::Float4: return GL_FLOAT;

        case ShaderDataType::Mat3:   return GL_FLOAT;
        case ShaderDataType::Mat4:   return GL_FLOAT;

        case ShaderDataType::Int:   return GL_INT;
        case ShaderDataType::Int2:  return GL_INT;
        case ShaderDataType::Int3:  return GL_INT;
        case ShaderDataType::Int4:  return GL_INT;

        case ShaderDataType::Bool: return GL_BOOL;
    }

    HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
    return -1;
}
// clang-format on

OpenGLVertexArray::OpenGLVertexArray()
{
    glCreateVertexArrays(1, &m_VertexArrayID);
    glBindVertexArray(m_VertexArrayID);
}
void OpenGLVertexArray::Bind()
{
    glBindVertexArray(m_VertexArrayID);
}
void OpenGLVertexArray::Unbind()
{
    glBindVertexArray(0);
}
void OpenGLVertexArray::AddVertexBuffer(Ref<VertexBuffer> &vertex_buffer)
{
    HZ_CORE_ASSERT(vertex_buffer->GetLayout().GetElements().size(), "Vertex buffer has no layout!");

    glBindVertexArray(m_VertexArrayID);
    vertex_buffer->Bind();

    uint32_t    index  = 0;
    const auto &layout = vertex_buffer->GetLayout();
    for (const auto &elem : layout.GetElements())
    {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index,
                              elem.GetComponentsCount(),
                              ShaderDataTypeToOpenGLBaseType(elem.Type),
                              elem.bNormalized ? GL_TRUE : GL_FALSE,
                              layout.GetStride(),
                              (const void *)elem.Offset);
        ++index;
    }

    m_VertexBuffers.push_back(vertex_buffer);
}

void OpenGLVertexArray::SetIndexBuffer(Ref<IndexBuffer> &index_buffer)
{
    glBindVertexArray(m_VertexArrayID);
    index_buffer->Bind();
    m_IndexBuffer = index_buffer;
}
OpenGLVertexArray::~OpenGLVertexArray()
{
    glDeleteVertexArrays(1, &m_VertexArrayID);
}
} // namespace hazel