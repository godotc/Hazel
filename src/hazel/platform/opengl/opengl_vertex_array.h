//
// Created by nono on 9/20/23.
//

#ifndef HAZEL_OPENGL_VERTEX_ARRAY_H
#define HAZEL_OPENGL_VERTEX_ARRAY_H


#include "hazel/renderer/vertex_array.h"

#include <memory>
#include <vector>

namespace hazel {

class OpenGLVertexArray : public VertexArray
{
  public:
    OpenGLVertexArray();
    ~OpenGLVertexArray() override;

    void Bind() override;
    void Unbind() override;

    void                                                     AddVertexBuffer(std::shared_ptr<VertexBuffer> &vertex_buffer) override;
    void                                                     SetIndexBuffer(std::shared_ptr<IndexBuffer> &index_buffer) override;
    inline const std::vector<std::shared_ptr<VertexBuffer>> &GetVertexBuffer() const override { return m_VertexBuffers; }
    inline const std::shared_ptr<IndexBuffer>               &GetIndexBuffer() const override { return m_IndexBuffer; }

  private:
    std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
    std::shared_ptr<IndexBuffer>               m_IndexBuffer;

    uint32_t m_VertexArrayID = 0;
};

} // namespace hazel

#endif // HAZEL_OPENGL_VERTEX_ARRAY_H
