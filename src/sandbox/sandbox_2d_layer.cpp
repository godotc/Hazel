//
// Created by nono on 10/11/23.
#include "hazel/scene/entity.h"
#include "hz_pch.h"

#include "hazel/core/base.h"
#include "hazel/scene/component.h"
#include "hazel/scene/scene.h"

#include "hazel/renderer/framebuffer.h"

#include "glm/detail/qualifier.hpp"
#include "glm/ext/vector_float2.hpp"
#include "hazel/renderer/subtexture_2d.h"
#include "hazel/renderer/texture.h"

#include "glm/fwd.hpp"
#include "hazel/core/input.h"
#include "hazel/renderer/render_2d.h"

#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "sandbox_2d_layer.h"


void Sandbox2D::OnAttach()
{

    hazel::FramebufferSpecification spec;
    spec.Width    = 1280;
    spec.Height   = 720;
    m_Framebuffer = hazel::Framebuffer::Create(spec);


    m_ActiveScene = hazel::CreateRef<hazel::Scene>();


    auto square = m_ActiveScene->CreateEntity("squre");

    square.AddComponent<hazel::SpriteRendererComponent>(glm::vec4{1, 0, 0, 0});
    m_SquareEntity = square;


    Init();
}

void Sandbox2D::OnDetach()
{
    hazel::Layer::OnDetach();
}

void Sandbox2D::OnUpdate(hazel::Timestep timestep)
{
    HZ_PROFILE_SCOPE("Sandbox2d::OnUpdate");

    m_Framebuffer->Bind();
    {

        hazel::Render2D::ResetStats();

        {
            HZ_PROFILE_SCOPE("Renderer Prep");
            hazel::RenderCommand::SetClearColor(m_ClearColor);
            hazel::RenderCommand::Clear();
        }



        {
            HZ_PROFILE_SCOPE("Renderer Draw-Calls");
            hazel::Render2D::BeginScene(m_CameraController.GetCamera());

            // hazel::Render2D::DrawRotateQuad(m_QuadPosition + glm::vec3{5, 5, 0}, {1, 1}, glm::radians(36.f), m_ArchTexture);
            // hazel::Render2D::DrawRotateQuad(m_QuadPosition + glm::vec3{10, 10, 1}, {2, 2}, glm::radians(18.f), {1, 0, 0, 1});
            // hazel::Render2D::DrawQuad({0, 0, -0.1}, {10, 10}, m_BlockTexture, 10);

            m_CameraController.OnUpdate(timestep);

            m_ActiveScene->OnUpdate(timestep);

            // tilesheet sample
            {
                hazel::Render2D::DrawQuad({2, 1, -0.1}, {1, 1}, m_TinyTownSheet, 1);

                for (int i = 0; i < 12; ++i) {
                    for (int j = 0; j < 11; ++j) {
                        const float size = 0.1f;
                        glm::vec2   pos  = glm::vec2{2, 2} + glm::vec2{i * size, j * size};
                        hazel::Render2D::DrawQuad(pos, {size, size}, m_SubBlock[i][j]);
                    }
                }
                hazel::Render2D::DrawQuad({3, 3}, {1, 1}, m_WaterBuck);
                hazel::Render2D::DrawQuad({3, 4}, {1, 2}, m_Tree);
            }

            // game map
            {
                // base paint
                for (int i = 0; i < game_map.size(); ++i) {
                    int x         = i % map_width;
                    int y         = i / map_width;
                    auto [tx, ty] = tile_block_map['w'];

                    hazel::Render2D::DrawQuad(
                        {-x, -y, -0.3},
                        {1, 1},
                        m_SubBlock[tx][ty]);
                }

                for (int i = 0; i < game_map.size(); ++i) {
                    // printf("%d\n", game_map.size());
                    int x         = i % map_width;
                    int y         = i / map_width;
                    auto [tx, ty] = tile_block_map.find(game_map[i])->second;

                    hazel::Render2D::DrawQuad(
                        {-x, -y},
                        {1, 1},
                        m_SubBlock[tx][ty]);
                }
            }

            hazel::Render2D::EndScene();
        }
    }
    m_Framebuffer->Unbind();
}


void Sandbox2D::OnImGuiRender()
{

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());


    if (ImGui::Begin("Settings")) {
        ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_ClearColor));
        ImGui::DragFloat3("Quad Position", glm::value_ptr(m_QuadPosition));
        ImGui::ColorEdit4("Flat Color", glm::value_ptr(m_FlatColor));
        ImGui::InputFloat2("Shift of tilemap", m_Shiftting);
        auto id = m_ArchTexture->GetTextureID();
        ImGui::Image((void *)id, ImVec2{64, 64});



        if (m_SquareEntity) {
            ImGui::Separator();
            auto &squre_color = m_SquareEntity.GetComponent<hazel::SpriteRendererComponent>().Color;
            ImGui::ColorEdit4("Square color", glm::value_ptr(squre_color));
        }

        ImGui::End();
    }


    if (ImGui::Begin("Render2D stats")) {
        auto stat = hazel::Render2D::GetStatics();
        ImGui::Text("Draw Calls  : %d", stat.DrawCalls);
        ImGui::Text("Quad Count  : %d", stat.QuadCount);
        ImGui::Text("Vertex Count: %d", stat.GetTotalVertexCount());
        ImGui::Text("Index Count : %d", stat.GetTotalIndexCount());
        auto [x, y] = hazel::Input::GetMousePos();
        ImGui::Text("Mouse Pos : %d,%d", (int)x, (int)y);
        ImGui::End();
    }


    if (ImGui::Begin("ViewPort", nullptr, ImGuiWindowFlags_NoScrollWithMouse)) {
        uint32_t framebuffer_colorattachmetn = m_Framebuffer->GetColorAttachmentID();
        ImGui::Image((void *)framebuffer_colorattachmetn, ImVec2{1280, 720});
        ImGui::End();
    }
}

void Sandbox2D::Init()
{
    m_CameraController.SetZoomLevel(3);

    m_FaceTexture  = hazel::Texture2D::Create(FPath("res/texture/face.png"));
    m_ArchTexture  = hazel::Texture2D::Create(FPath("res/texture/arch.png"));
    m_BlockTexture = hazel::Texture2D::Create(FPath("res/texture/block.png"));

    m_TinyTownSheet = hazel::Texture2D::Create(FPath("res/texture/tiny_town/tilemap_packed.png"));
    for (int i = 0; i < 12; ++i) {
        for (int j = 0; j < 11; ++j) {
            m_SubBlock[i][j] = hazel::SubTexture2D::CreateFromCoords(
                m_TinyTownSheet,
                {i, j},
                {16.f, 16.f},
                {16.f, 16.f});
        }
    }
    m_WaterBuck = hazel::SubTexture2D::CreateFromCoords(m_TinyTownSheet, {11, 0}, {16.f, 16.f}, {16.f, 16.f});

    m_Tree = hazel::SubTexture2D::CreateFromCoords(
        m_TinyTownSheet,
        {4, -2},
        {16.f, 16.f},
        {16.f, 32.f});



    m_PracticleProps.ColorBegin = {254 / 255.f, 212 / 255.f, 123 / 244.f, 1.f};
    m_PracticleProps.ColorEnd   = {254 / 255.f, 109 / 255.f, 41 / 244.f, 1.f};

    m_PracticleProps.SizeBegin     = 0.5f,
    m_PracticleProps.SizeEnd       = 0.f;
    m_PracticleProps.SizeVariation = 0.3f;

    m_PracticleProps.Velocity          = {0.f, 0.f};
    m_PracticleProps.VelocityVariation = {4.f, 1.5f};

    m_PracticleProps.Position = {0.f, 0.f};
}

void Sandbox2D::OnEvent(hazel::Event &event)
{
    m_CameraController.OnEvent(event);
}
