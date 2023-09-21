//
// Created by nono on 9/20/23.
//

#ifndef HAZEL_OPENGL_VERTEX_ARRAY_H
#define HAZEL_OPENGL_VERTEX_ARRAY_H


#include "hazel/renderer/vertex_array.h"
namespace hazel {

class OpenGLVertexArray : public VertexArray
{
  public:
    OpenGLVertexArray();

    void Bind() override;
    void Unbind() override;

    void AddVertexBuffer(std::shared_ptr<VertexBuffer> &vertex_buffer) const override;
    void SetIndexBuffer(std::shared_ptr<VertexBuffer> &vertex_buffer) const override;

  private:
    std::vector<std::shared_ptr<VertexBuffer> > m_VertexBuffers;
    std::shared_ptr<IndexBuffer>                m_IndexBuffers;

    uint32_t m_VertexArrayID;
};

} // namespace hazel

#endif // HAZEL_OPENGL_VERTEX_ARRAY_H
