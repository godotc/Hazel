//
// Created by nono on 9/22/23.
//

#include "render_command.h"

#include "hazel/platform/opengl/opengl_render_api.h"

namespace hazel {

RendererAPI *RenderCommand::Get()
{
    static RendererAPI *api = new OpenGLRenderAPI();
    return api;
}

void RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    Get()->SetViewPort(x, y, w, h);
}
} // namespace hazel