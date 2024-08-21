//
// Created by nono on 9/22/23.
//

#include "render_command.h"
#include "platform/opengl/opengl_render_api.h"

namespace hazel {

hazel::RendererAPI *RenderCommand::GetRenderAPI()
{
    static RendererAPI *api = new OpenGLRenderAPI();
    return api;
}

} // namespace hazel