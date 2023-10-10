//
// Created by nono on 9/20/23.
//
#include "hz_pch.h"

#include "render.h"
#include "vertex_array.h"

#include "hazel/hazel/core/log.h"
#include "platform/opengl/opengl_vertex_array.h"

namespace hazel {
VertexArray *VertexArray::Create()
{
    switch (Render::GetRendererAPI()) {
    case RendererAPI::API::NONE:
        HZ_CORE_ASSERT(false, "RendererAPI::None is current not supported!");
        return nullptr;
    case RendererAPI::API::OPENGL:
        return new OpenGLVertexArray;
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