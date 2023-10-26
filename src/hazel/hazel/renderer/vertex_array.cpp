//
// Created by nono on 9/20/23.
//

#include "vertex_array.h"
#include "render.h"

#include "hazel/core/log.h"
#include "platform/opengl/opengl_vertex_array.h"

namespace hazel {
Ref<VertexArray> VertexArray::Create()
{
    switch (Render::GetRendererAPI()) {
        case RendererAPI::API::NONE:
            HZ_CORE_ASSERT(false, "RendererAPI::None is current not supported!");
            return nullptr;
        case RendererAPI::API::OPENGL:
            return std::make_shared<OpenGLVertexArray>();
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