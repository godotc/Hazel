//
// Created by nono on 10/11/23.
//

#include "hz_pch.h"

#include "glm/gtc/type_ptr.hpp"
#include "platform/opengl/opengl_shader.h"
#include "sandbox_2d_layer.h"


void Sandbox2D::OnAttach()
{

    m_Texture     = hazel::Texture2D::Create(FPath("res/texture/face.png"));
    m_ArchTexture = hazel::Texture2D::Create(FPath("res/texture/arch.png"));
}

void Sandbox2D::OnDetach()
{
    Layer::OnDetach();
}

void Sandbox2D::OnUpdate(hazel::Timestep timestep)
{
    m_CameraController.OnUpdate(timestep);

    hazel::RenderCommand::SetClearColor(m_ClearColor);
    hazel::RenderCommand::Clear();

    hazel::Render2D::BeginScene(m_CameraController.GetCamera());

    //    if (auto ogl_shader = dynamic_pointer_cast<hazel::OpenGLShader>(m_FlatShader)) {
    //        ogl_shader->Bind();
    //        ogl_shader->UploadUniformFloat4("u_Color", m_FlatColor);
    //    }
    //
    //    hazel::Render::Submit(m_FlatShader, m_FlatVA, glm::mat4(1.f));
    hazel::Render2D::DrawQuad(m_QuadPosition, {2, 2}, m_FlatColor);

    hazel::Render2D::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_ClearColor));
    ImGui::DragFloat3("Quad Position", glm::value_ptr(m_QuadPosition));
    ImGui::ColorEdit4("Flat Color", glm::value_ptr(m_FlatColor));
    ImGui::End();
}
void Sandbox2D::OnEvent(hazel::Event &event)
{
    m_CameraController.OnEvent(event);
}
