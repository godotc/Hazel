//
// Created by nono on 9/20/23.
//

#include "render.h"
#include "orthographic_camera.h"
#include "platform/opengl/opengl_shader.h"
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
void Render::Submit(const Ref<Shader> &shader, Ref<VertexArray> &vertex_array, const glm::mat4 transform)
{
    shader->Bind();

    std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
    std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);
    vertex_array->Bind();
    RenderCommand::DrawIndex(vertex_array);
}
void Render::Init()
{
    RenderCommand::Init();
}
} // namespace hazel