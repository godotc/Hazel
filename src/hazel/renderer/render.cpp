//
// Created by nono on 9/20/23.
//

#include "render.h"
#include "render_command.h"

namespace hazel {


void Render::BeginScene()
{
}
void Render::EndScene()
{
}

void Render::Submit(std::shared_ptr<VertexArray> &vertex_array)
{
    vertex_array->Bind();
    RenderCommand::DrawIndex(vertex_array);
}
} // namespace hazel