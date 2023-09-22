//
// Created by nono on 9/20/23.
//

#include "render.h"
#include "orthographic_camera.h"
#include "render_command.h"
#include "shader.h"

namespace hazel {

Render::SceneData *Render::m_SceneData = new Render::SceneData;

void Render::BeginScene(OrthographicsCamera &Camera)
{
    m_SceneData->ViewProjectionMatrix = Camera.GetViewProjectionMatrix();
}

void Render::EndScene()
{
}
void Render::Submit(const std::shared_ptr<Shader> &shader, std::shared_ptr<VertexArray> &vertex_array)
{
    shader->Bind();
    shader->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
    vertex_array->Bind();
    RenderCommand::DrawIndex(vertex_array);
}
} // namespace hazel