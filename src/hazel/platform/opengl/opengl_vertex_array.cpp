//
// Created by nono on 9/20/23.
//

#include "hz_pch.h"

#include "glad/glad.h"
#include "opengl_vertex_array.h"

namespace hazel {


//static GLenum ShaderDataTypeToOpenGLBaseType(ShaderdataTye type)
//{
//}

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
void OpenGLVertexArray::AddVertexBuffer(std::shared_ptr<VertexBuffer> &vertex_buffer) const
{
    glBindVertexArray(m_VertexArrayID);
    vertex_buffer->Bind();
}
void OpenGLVertexArray::SetIndexBuffer(std::shared_ptr<VertexBuffer> &vertex_buffer) const
{
}
} // namespace hazel