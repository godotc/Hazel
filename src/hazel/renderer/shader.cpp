//
// Created by nono on 9/20/23.
//
#include "hz_pch.h"
#include "renderer/render.h"

#include "shader.h"

#include "platform/opengl/opengl_shader.h"

namespace hazel {



Shader *Shader::Create(const std::string &vert_src, const std::string &frag_src, const std::string &geom_src)
{
    switch (Render::GetRendererAPI()) {
    case RendererAPI::API::NONE:
        HZ_CORE_ASSERT(false, "RendererAPI::None is current not supported!");
        return nullptr;
    case RendererAPI::API::OPENGL:
        return new OpenGLShader(vert_src, frag_src, geom_src);
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