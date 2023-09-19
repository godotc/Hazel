//
// Created by nono on 9/20/23.
//

#ifndef HAZEL_OPEN_GL_BUFFER_H
#define HAZEL_OPEN_GL_BUFFER_H

#include "hazel/renderer/buffer.h"
#include <cstdint>

namespace hazel {

class OpenGLVertexBuffer : public VertexBuffer
{
  public:
    OpenGLVertexBuffer(float *vertices, uint32_t size);
    ~OpenGLVertexBuffer() override;

    void Bind() override;
    void Unbind() override;

  private:
    uint32_t m_BufferID;
};

class OpenGLIndexBuffer : public IndexBuffer
{
  public:
    OpenGLIndexBuffer(uint32_t *indices, uint32_t count);
    ~OpenGLIndexBuffer() override;

    void Bind() override;
    void Unbind() override;

  private:
    uint32_t m_BufferID;
};

} // namespace hazel

#endif // HAZEL_OPEN_GL_BUFFER_H
