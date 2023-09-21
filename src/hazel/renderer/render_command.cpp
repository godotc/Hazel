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
} // namespace hazel