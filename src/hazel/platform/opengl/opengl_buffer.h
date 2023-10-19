//
// Created by nono on 9/20/23.
//

#ifndef HAZEL_OPENGL_BUFFER_H
#define HAZEL_OPENGL_BUFFER_H

#include "hazel/renderer/buffer.h"
#include <cstdint>

namespace hazel {

class OpenGLVertexBuffer : public VertexBuffer
{
  public:
    OpenGLVertexBuffer(float *vertices, uint32_t size);
    OpenGLVertexBuffer(uint32_t size);
    ~OpenGLVertexBuffer() override;

    inline void          SetLayout(const hazel::BufferLayout &layout) override { m_Layout = layout; }
    inline BufferLayout &GetLayout() override { return m_Layout; }

    void SetData(void *data, uint32_t size) override;


    void Bind() override;
    void Unbind() override;

  private:
    uint32_t     m_BufferID;
    BufferLayout m_Layout = {};
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

#endif // HAZEL_OPENGL_BUFFER_H
