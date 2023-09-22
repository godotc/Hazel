//
// Created by nono on 9/20/23.
//

#ifndef HAZEL_VERTEX_ARRAY_H
#define HAZEL_VERTEX_ARRAY_H

#include <hazel/core.h>

#include <memory>

namespace hazel {

class VertexBuffer;
class IndexBuffer;

class VertexArray
{
  public:
    virtual ~VertexArray() = default;

    virtual void Bind()   = 0;
    virtual void Unbind() = 0;

    virtual void AddVertexBuffer(std::shared_ptr<VertexBuffer> &vertex_buffer) = 0;
    virtual void SetIndexBuffer(std::shared_ptr<IndexBuffer> &index_buffer)    = 0;

    virtual const std::vector<std::shared_ptr<VertexBuffer>> &GetVertexBuffer() const = 0;
    virtual const std::shared_ptr<IndexBuffer>               &GetIndexBuffer() const  = 0;

    static VertexArray *Create();
};

} // namespace hazel

#endif // HAZEL_VERTEX_ARRAY_H
