//
// Created by nono on 9/20/23.
//
#include "hz_pch.h"

#include "shader.h"

#include "renderer/render.h"


#include "platform/opengl/opengl_shader.h"

namespace hazel {



Ref<Shader> Shader::Create(const std::string &name, const std::string &vert_src, const std::string &frag_src)
{
    switch (Render::GetRendererAPI()) {
        case RendererAPI::API::NONE:
            HZ_CORE_ASSERT(false, "RendererAPI::None is current not supported!");
            return nullptr;
        case RendererAPI::API::OPENGL:
            return Ref<Shader>(new OpenGLShader(name, vert_src, frag_src));
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

Ref<Shader> Shader::Create(const std::string &shader_file_path)
{
    switch (Render::GetRendererAPI()) {
        case RendererAPI::API::NONE:
            HZ_CORE_ASSERT(false, "RendererAPI::None is current not supported!");
            return nullptr;
        case RendererAPI::API::OPENGL:
            return Ref<Shader>(new OpenGLShader(shader_file_path));
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
    return nullptr;
}
Ref<Shader> ShaderLibrary::Get(const std::string &name)
{
    HZ_CORE_ASSERT(Exist(name), "Shader not found!");
    return m_Shaders[name];
}

void ShaderLibrary::Add(const std::string &name, const Ref<Shader> &shader)
{
    //    HZ_CORE_ASSERT(!Exist(name), "Shader already exist");
    m_Shaders.insert({name, shader});
}
void ShaderLibrary::Add(const Ref<Shader> &shader)
{
    auto &name = shader->GetName();
    Add(name, shader);
}
Ref<Shader> ShaderLibrary::Load(const std::string &file_path)
{
    auto shader = Shader::Create(file_path);
    Add(shader);
    return shader;
}
Ref<Shader> ShaderLibrary::Load(const std::string &name, const std::string &file_path)
{
    // TODO: Maybe conflicts between the filename and custom-name
    auto shader = Shader::Create(file_path);
    Add(name, shader);
    return shader;
}
bool ShaderLibrary::Exist(const std::string &name) const
{
    return m_Shaders.find(name) != m_Shaders.end();
}

} // namespace hazel