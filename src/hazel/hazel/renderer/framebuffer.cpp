#pragma once
#include "framebuffer.h"
#include "platform/opengl/opengl_framebuffer.h"
#include "render.h"

namespace hazel {

Ref<Framebuffer> Framebuffer::Create(const FramebufferSpec &spec)
{
    switch (Render::GetRendererAPI()) {
        case RendererAPI::API::NONE:
            HZ_CORE_ASSERT(false, "RendererAPI::None is current not supported!");
            return nullptr;
        case RendererAPI::API::OPENGL:
            return CreateRef<OpenGLFrameBuffer>(spec);
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