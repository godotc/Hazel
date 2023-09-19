//
// Created by nono on 9/20/23.
//

#ifndef HAZEL_BUFFER_H
#define HAZEL_BUFFER_H

namespace hazel {



class VertexBuffer
{
  public:
    virtual ~VertexBuffer() = default;

    virtual void Bind()   = 0;
    virtual void Unbind() = 0;

    static VertexBuffer *Create(float *vertices, uint32_t size);
};

class IndexBuffer
{
  public:
    virtual ~IndexBuffer() = default;

    virtual void Bind()   = 0;
    virtual void Unbind() = 0;

    virtual uint32_t GetCount() const { return m_Count; };

    static IndexBuffer *Create(uint32_t *indeices, uint32_t count);

    uint32_t m_Count;
};

} // namespace hazel

#endif // HAZEL_BUFFER_H
