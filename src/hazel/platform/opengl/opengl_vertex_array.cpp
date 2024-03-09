//
// Created by nono on 9/20/23.
//

#include "hz_pch.h"

#include "hazel/core/base.h"

#include "glad/glad.h"
#include "hazel/renderer/buffer.h"
#include "opengl_vertex_array.h"
#include <cstdint>



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
    HZ_PROFILE_FUNCTION();

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
    HZ_PROFILE_FUNCTION();

    HZ_CORE_ASSERT(vertex_buffer->GetLayout().GetElements().size(), "Vertex buffer has no layout!");

    glBindVertexArray(m_VertexArrayID);
    vertex_buffer->Bind();

    const auto &layout = vertex_buffer->GetLayout();

    for (const auto &elem : layout.GetElements())
    {
        switch (elem.Type) {

            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
            {
                glEnableVertexAttribArray(m_VertexBufferAttrIndex);
                glVertexAttribPointer(m_VertexBufferAttrIndex,
                                      elem.GetComponentsCount(),
                                      ShaderDataTypeToOpenGLBaseType(elem.Type),
                                      elem.bNormalized ? GL_TRUE : GL_FALSE,
                                      layout.GetStride(),
                                      reinterpret_cast<const void *>(elem.Offset));
                ++m_VertexBufferAttrIndex;
                break;
            }
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
            case ShaderDataType::Bool:
            {
                glEnableVertexAttribArray(m_VertexBufferAttrIndex);
                glVertexAttribIPointer(m_VertexBufferAttrIndex,
                                       elem.GetComponentsCount(),
                                       ShaderDataTypeToOpenGLBaseType(elem.Type),
                                       layout.GetStride(),
                                       reinterpret_cast<const void *>(elem.Offset));
                ++m_VertexBufferAttrIndex;
                break;
            }
            case ShaderDataType::Mat3:
            case ShaderDataType::Mat4:
            {
                uint8_t count = elem.GetComponentsCount();
                // TODO: WHY DO THIS?
                for (uint8_t i = 0; i < count; ++i) {
                    glEnableVertexAttribArray(m_VertexBufferAttrIndex);
                    glVertexAttribPointer(m_VertexBufferAttrIndex,
                                          count,
                                          elem.bNormalized ? GL_TRUE : GL_FALSE,
                                          ShaderDataTypeToOpenGLBaseType(elem.Type),
                                          layout.GetStride(),
                                          reinterpret_cast<const void *>(elem.Offset + sizeof(float) * count * i));
                    // TODO: WHY CALL THIS?
                    glVertexAttribDivisor(m_VertexBufferAttrIndex, 1);
                    ++m_VertexBufferAttrIndex;
                }
                break;
            }
            default:
                HZ_CORE_ASSERT(false, "Unknow shader data type!");
        }
    }

    m_VertexBuffers.push_back(vertex_buffer);
}

void OpenGLVertexArray::SetIndexBuffer(Ref<IndexBuffer> &index_buffer)
{
    HZ_PROFILE_FUNCTION();

    glBindVertexArray(m_VertexArrayID);
    index_buffer->Bind();
    m_IndexBuffer = index_buffer;
}
OpenGLVertexArray::~OpenGLVertexArray()
{
    glDeleteVertexArrays(1, &m_VertexArrayID);
}
} // namespace hazel