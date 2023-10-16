//
// Created by nono on 10/11/23.
//

#include "hz_pch.h"

#include "glm/gtc/type_ptr.hpp"
#include "platform/opengl/opengl_shader.h"
#include "sandbox_2d_layer.h"


void Sandbox2D::OnAttach()
{
    m_CameraController.SetZoomLevel(3);

    m_FaceTexture  = hazel::Texture2D::Create(FPath("res/texture/face.png"));
    m_ArchTexture  = hazel::Texture2D::Create(FPath("res/texture/arch.png"));
    m_BlockTexture = hazel::Texture2D::Create(FPath("res/texture/block.png"));
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

    //    hazel::Render2D::DrawQuad(m_QuadPosition, {2, 2}, m_FlatColor);
    //    hazel::Render2D::DrawQuad(m_QuadPosition + glm::vec3{2, 2, 0}, {1, 1}, {1.f, 0.1f, 0.1f, 0});

    hazel::Render2D::DrawQuad(m_QuadPosition + glm::vec3{3, 3, 0}, {1, 1}, m_FaceTexture);
    hazel::Render2D::DrawQuad(m_QuadPosition + glm::vec3{5, 5, 0}, {1, 1}, m_ArchTexture);

    hazel::Render2D::DrawQuad({0, 0, -0.9}, {10, 10}, m_BlockTexture);

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
