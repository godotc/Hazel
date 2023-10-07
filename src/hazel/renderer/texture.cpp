//
// Created by nono on 9/24/23.
//

#include "hz_pch.h"

#include "render.h"
#include "texture.h"

#include <memory>

#include "platform/opengl/opengl_texture.h"

namespace hazel {
Ref<Texture2D> Texture2D::Create(const std::string &path)
{
    switch (Render::GetRendererAPI()) {
    case RendererAPI::API::NONE:
        HZ_CORE_ASSERT(false, "RendererAPI::None is current not supported!");
        return nullptr;
    case RendererAPI::API::OPENGL:
        return std::make_shared<OpenGLTexture2D>(path);
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