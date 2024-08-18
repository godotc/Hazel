//
// Created by nono on 9/20/23.
//

#ifndef HAZEL_OPENGL_VERTEX_ARRAY_H
#define HAZEL_OPENGL_VERTEX_ARRAY_H


#include "hazel/renderer/vertex_array.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace hazel {

class OpenGLVertexArray : public VertexArray
{
    uint32_t                       m_VertexArrayID = 0;
    std::vector<Ref<VertexBuffer>> m_VertexBuffers;
    Ref<IndexBuffer>               m_IndexBuffer;

    uint32_t m_VertexBufferAttrIndex = 0;

  public:
    OpenGLVertexArray();
    ~OpenGLVertexArray() override;

    void Bind() override;
    void Unbind() override;

    void                                         AddVertexBuffer(Ref<VertexBuffer> vertex_buffer) override;
    void                                         SetIndexBuffer(Ref<IndexBuffer> index_buffer) override;
    Ref<IndexBuffer>                             GetIndexBuffer() override { return m_IndexBuffer; }
    inline const std::vector<Ref<VertexBuffer>> &GetVertexBuffer() const override { return m_VertexBuffers; }
    inline const Ref<IndexBuffer>               &GetIndexBuffer() const override { return m_IndexBuffer; }
};

} // namespace hazel

#endif // HAZEL_OPENGL_VERTEX_ARRAY_H
