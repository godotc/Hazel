
#include "opengl_uniform_buffer.h"
#include <glad/glad.h>

namespace hazel {

OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
{
    // Create means not bind to target(vertex_array)
    glCreateBuffers(1, &m_UniformBufferID);
    glNamedBufferData(m_UniformBufferID, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_UniformBufferID);
}

OpenGLUniformBuffer::~OpenGLUniformBuffer()
{
    glDeleteBuffers(1, &m_UniformBufferID);
}

void OpenGLUniformBuffer::SetData(const void *data, uint32_t size, uint32_t offset)
{
    glNamedBufferSubData(m_UniformBufferID, offset, size, data);
}

} // namespace hazel
