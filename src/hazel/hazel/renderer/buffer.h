//
// Created by nono on 9/20/23.
//

#ifndef HAZEL_BUFFER_H
#define HAZEL_BUFFER_H

#include <cstdint>
#include <utility>
#include <vector>

#include "hazel/core/core.h"

namespace hazel {

// clang-format off
enum class ShaderDataType
{
    None = 0,
    Float, Float2, Float3, Float4,
    Mat3, Mat4,
    Int, Int2, Int3, Int4,
    Bool
};

static uint32_t ShaderDataTypeSize(ShaderDataType type)
{
    switch (type) {
        case ShaderDataType::Float:  return 4;
        case ShaderDataType::Float2: return 4 * 2;
        case ShaderDataType::Float3: return 4 * 3;
        case ShaderDataType::Float4: return 4 * 4;

        case ShaderDataType::Mat3:   return 4 * 3 * 3;
        case ShaderDataType::Mat4:   return 4 * 4 * 4;

        case ShaderDataType::Int:   return 4;
        case ShaderDataType::Int2:  return 4 * 2;
        case ShaderDataType::Int3:  return 4 * 3;
        case ShaderDataType::Int4:  return 4 * 4;

        case ShaderDataType::Bool: return 1;
    }

    HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
    return 0;
}

struct BufferElement {
    std::string    Name;
    ShaderDataType Type;
    bool           bNormalized;
    uint32_t       Size;
    uint32_t       Offset;

    BufferElement(ShaderDataType type, std::string name, bool bNormalized = false)
        : Name(std::move(name)), Type(type),bNormalized(bNormalized), Size(ShaderDataTypeSize(type)), Offset(0) {}

    [[nodiscard]] uint32_t GetComponentsCount() const
    {
        switch (Type) {
            case ShaderDataType::Float: return 1;
            case ShaderDataType::Float2: return 2;
            case ShaderDataType::Float3: return  3;
            case ShaderDataType::Float4: return  4;

            case ShaderDataType::Mat3: return  3 * 3;
            case ShaderDataType::Mat4: return  4 * 4;

            case ShaderDataType::Int:  return 1;
            case ShaderDataType::Int2: return  2;
            case ShaderDataType::Int3: return  3;
            case ShaderDataType::Int4: return  4;

            case ShaderDataType::Bool: return 1;
        }

        HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }
};
// clang-format on

class BufferLayout
{
  public:
    BufferLayout(std::initializer_list<BufferElement> &&elements) : m_Elements(elements) { CalculateOffsetsAndStride(); }
    BufferLayout(std::initializer_list<BufferElement> &elements) : m_Elements(elements) { CalculateOffsetsAndStride(); }

  public:
    [[nodiscard]] inline const std::vector<BufferElement> GetElements() const { return m_Elements; }
    [[nodiscard]] inline const uint32_t                   GetStride() const { return m_Stride; }

  private:
    void CalculateOffsetsAndStride()
    {
        uint32_t offset = 0;
        m_Stride        = 0;
        for (auto &ele : m_Elements)
        {
            ele.Offset = offset;
            offset += ele.Size;
            m_Stride += ele.Size;
        }
    }

  private:
    std::vector<BufferElement> m_Elements;
    uint32_t                   m_Stride = {0};
};


class VertexBuffer
{
  public:
    virtual ~VertexBuffer() = default;

    virtual void Bind()   = 0;
    virtual void Unbind() = 0;

    virtual void          SetLayout(const BufferLayout &layout) = 0;
    virtual BufferLayout &GetLayout()                           = 0;

    static Ref<VertexBuffer> Create(float *vertices, uint32_t size);
    static Ref<VertexBuffer> Create(uint32_t size);

    virtual void SetData(void *data, uint32_t size) = 0;
};

// 32bit currently
class IndexBuffer
{
  public:
    virtual ~IndexBuffer() = default;

    virtual void Bind()   = 0;
    virtual void Unbind() = 0;

    [[nodiscard]] virtual uint32_t GetCount() const { return m_Count; };

    static Ref<IndexBuffer> Create(uint32_t *indices, uint32_t count);

    uint32_t m_Count;
};

} // namespace hazel

#endif // HAZEL_BUFFER_H
