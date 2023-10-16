//
// Created by nono on 9/20/23.
//

#include "hz_pch.h"

#include "buffer.h"
#include "hazel/hazel/core/core.h"
#include "hazel/hazel/core/log.h"
#include "hazel/platform/opengl/opengl_buffer.h"
#include "render.h"

namespace hazel {
VertexBuffer *VertexBuffer::Create(float *vertices, uint32_t size)
{
    switch (Render::GetRendererAPI()) {
    case RendererAPI::API::NONE:
        HZ_CORE_ASSERT(false, "RendererAPI::None is current not supported!");
        return nullptr;
    case RendererAPI::API::OPENGL:
        return new OpenGLVertexBuffer(vertices, size);
        //    case RendererAPI::VULKAN:
        //        break;
        //    case RendererAPI::DIRECTX:
        //        break;
        //    case RendererAPI::METAL:
        //        break;
    default:
        HZ_CORE_ASSERT(false, "Unknown render API is not supported!");
        return nullptr;
    };
}

IndexBuffer *IndexBuffer::Create(uint32_t *indices, uint32_t count)
{
    switch (Render::GetRendererAPI()) {
    case RendererAPI::API::NONE:
        HZ_CORE_ASSERT(false, "RendererAPI::None is current not supported!");
        return nullptr;
    case RendererAPI::API::OPENGL:
        return new OpenGLIndexBuffer(indices, count);
        //    case RendererAPI::VULKAN:
        //        break;
        //    case RendererAPI::DIRECTX:
        //        break;
        //    case RendererAPI::METAL:
        //        break;
    default:
        HZ_CORE_ASSERT(false, "Unknown render API is not supported!");
        return nullptr;
    };
}
} // namespace hazel