
#include "hz_pch.h"

#include "hazel/renderer/renderer_api.h"
#include "platform/opengl/opengl_uniform_buffer.h"
#include "uniform_buffer.h"



namespace hazel {

Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
{
    switch (RendererAPI::GetAPI())
    {
        case hazel::RendererAPI::API::NONE:
            HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OPENGL:
            return CreateRef<OpenGLUniformBuffer>(size, binding);
    }

    HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

} // namespace hazel