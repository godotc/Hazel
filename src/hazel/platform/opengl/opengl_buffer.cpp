//
// Created by nono on 9/20/23.
//
#include "hz_pch.h"

#include "opengl_buffer.h"

#include "glad/glad.h"

namespace hazel {

OpenGLVertexBuffer::OpenGLVertexBuffer(float *vertices, uint32_t size)
{
    HZ_PROFILE_FUNCTION();

    glGenBuffers(1, &m_BufferID);
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
{
    HZ_PROFILE_FUNCTION();

    glGenBuffers(1, &m_BufferID);
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
    glDeleteBuffers(1, &m_BufferID);
}

void OpenGLVertexBuffer::SetData(void *data, uint32_t size)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

void OpenGLVertexBuffer::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
}
void OpenGLVertexBuffer::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t *indices, uint32_t count)
{
    HZ_PROFILE_FUNCTION();

    m_Count = count;
    glGenBuffers(1, &m_BufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}
OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
    glDeleteBuffers(1, &m_BufferID);
}
void OpenGLIndexBuffer::Bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
}
void OpenGLIndexBuffer::Unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

} // namespace hazel