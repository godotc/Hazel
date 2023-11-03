//
// Created by nono on 10/11/23.
//

#include "glm/fwd.hpp"
#include "hazel/core/app.h"
#include "hazel/core/input.h"
#include "hazel/core/log.h"
#include "hazel/core/mouse_button.h"
#include "hazel/renderer/render_2d.h"
#include "hz_pch.h"

#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "sandbox_2d_layer.h"
#include <cmath>
#include <cstdlib>
#include <math.h>



void Sandbox2D::OnAttach()
{
    m_CameraController.SetZoomLevel(3);

    m_FaceTexture  = hazel::Texture2D::Create(FPath("res/texture/face.png"));
    m_ArchTexture  = hazel::Texture2D::Create(FPath("res/texture/arch.png"));
    m_BlockTexture = hazel::Texture2D::Create(FPath("res/texture/block.png"));


    m_PracticleProps.ColorBegin        = {254 / 255.f, 212 / 255.f, 123 / 244.f, 1.f};
    m_PracticleProps.ColorEnd          = {254 / 255.f, 109 / 255.f, 41 / 244.f, 1.f};
    m_PracticleProps.SizeBegin         = 0.5f,
    m_PracticleProps.SizeVariation     = 0.3f;
    m_PracticleProps.SizeEnd           = 0.f;
    m_PracticleProps.Velocity          = {0.f, 0.f};
    m_PracticleProps.VelocityVariation = {3.f, 1.f};
    m_PracticleProps.Position          = {0.f, 0.f};
}

void Sandbox2D::OnDetach()
{
    Layer::OnDetach();
}

void Sandbox2D::OnUpdate(hazel::Timestep timestep)
{
    HZ_PROFILE_SCOPE("Sandbox2d::OnUpdate");

    hazel::Render2D::ResetStats();

    {
        HZ_PROFILE_SCOPE("Renderer Prep");
        hazel::RenderCommand::SetClearColor(m_ClearColor);
        hazel::RenderCommand::Clear();
    }

    m_CameraController.OnUpdate(timestep);

    {
        HZ_PROFILE_SCOPE("Renderer Draw-Calls");
        hazel::Render2D::BeginScene(m_CameraController.GetCamera());

        hazel::Render2D::DrawQuad(m_QuadPosition + glm::vec3{0, 0, 0.1}, {2, 2}, m_FlatColor);
        hazel::Render2D::DrawQuad(m_QuadPosition + glm::vec3{2, 2, 0.1}, {1, 1}, {1.f, 1.1f, 1.1f, 1.f});

        hazel::Render2D::DrawQuad(m_QuadPosition + glm::vec3{3, 3, 0}, {1, 1}, m_FaceTexture);
        hazel::Render2D::DrawRotateQuad(m_QuadPosition + glm::vec3{5, 5, 0}, {1, 1}, glm::radians(45.f), m_ArchTexture);
        hazel::Render2D::DrawQuad(m_QuadPosition + glm::vec3{2, 2, 0.1}, {1, 1}, {1.f, 1.1f, 1.1f, 1.f});

        hazel::Render2D::DrawQuad(m_QuadPosition + glm::vec3{3, 3, 0}, {1, 1}, m_FaceTexture);
        hazel::Render2D::DrawRotateQuad(m_QuadPosition + glm::vec3{5, 5, 0}, {1, 1}, glm::radians(45.f), m_ArchTexture);

        hazel::Render2D::DrawQuad(m_QuadPosition + glm::vec3{2, 2, 0.1}, {1, 1}, {1.f, 1.1f, 1.1f, 1.f});

        hazel::Render2D::DrawQuad(m_QuadPosition + glm::vec3{3, 3, 0}, {1, 1}, m_FaceTexture);

        hazel::Render2D::DrawRotateQuad(m_QuadPosition + glm::vec3{5, 5, 0}, {1, 1}, 36.f, m_ArchTexture);

        hazel::Render2D::DrawRotateQuad(m_QuadPosition + glm::vec3{10, 10, 1}, {2, 2}, 18.f, {1, 0, 0, 1});
        hazel::Render2D::DrawRotateQuad(m_QuadPosition + glm::vec3{5, 5, 1}, {2, 2}, 18.f, {1, 0, 0, 1});

        hazel::Render2D::DrawQuad({0, 0, -0.1}, {10, 10}, m_BlockTexture, 10);
        hazel::Render2D::EndScene();



        hazel::Render2D::BeginScene(m_CameraController.GetCamera());
        for (float y = -5.f; y < 5.f; y += 0.5f) {
            for (float x = -5.f; x < 5.f; x += 0.5f) {
                glm::vec2 rb    = {sin(x), cos(y * x)};
                glm::vec4 color = glm::vec4(rb, tan(y), 0.3f);

                hazel::Render2D::DrawQuad(glm::vec2{x, y}, {1, 1}, color);
            }
        }
        hazel::Render2D::EndScene();
    }


    if (hazel::Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_LEFT)) {
        auto  mouse_pos = hazel::Input::GetMousePos();
        float x         = mouse_pos.first;
        float y         = mouse_pos.second;
        // HZ_INFO("{} {}", x, y);
        auto w = hazel::App::Get().GetWindow().GetWidth();
        auto h = hazel::App::Get().GetWindow().GetHeight();

        auto bounds = m_CameraController.GetBound();
        auto pos    = m_CameraController.GetCamera().GetPosition();

        x = (x / w) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
        y = bounds.GetHeight() * 0.5f - (y / h) * bounds.GetHeight();

        m_PracticleProps.Position = {x + pos.x, y + pos.y};
        HZ_INFO("Should be {} {}", x + pos.x, y + pos.y);

        auto i = 5;
        while (i--) {
            m_PracticleSystem.Emit(m_PracticleProps);
        }
    }

    m_PracticleSystem.OnUpdate(timestep);
    m_PracticleSystem.OnRender(m_CameraController.GetCamera());
}

void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_ClearColor));
    ImGui::DragFloat3("Quad Position", glm::value_ptr(m_QuadPosition));
    ImGui::ColorEdit4("Flat Color", glm::value_ptr(m_FlatColor));
    ImGui::End();


    ImGui::Begin("Render2D stats");
    auto stat = hazel::Render2D::GetStatics();
    ImGui::Text("Draw Calls  : %d", stat.DrawCalls);
    ImGui::Text("Quad Count  : %d", stat.QuadCount);
    ImGui::Text("Vertex Count: %d", stat.GetTotalVertexCount());
    ImGui::Text("Index Count : %d", stat.GetTotalIndexCount());
    auto [x, y] = hazel::Input::GetMousePos();
    ImGui::Text("Mouse Pos : %d,%d", (int)x, (int)y);
    ImGui::End();


    // ImGui::Begin("Profiling");
    // for (auto &result : m_ProfileResults | std::views::reverse)
    // {
    //     ImGui::Text("%-20s: %.3fms", result.Name, result.Time);
    // }
    // m_ProfileResults.clear();
    // ImGui::End();
}

void Sandbox2D::OnEvent(hazel::Event &event)
{
    m_CameraController.OnEvent(event);
}
