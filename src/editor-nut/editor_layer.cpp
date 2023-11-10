
#include "glm/ext/vector_float2.hpp"
#include "glm/gtc/type_ptr.hpp" #include "hazel/core/layer.h"
#include "hazel/core/log.h"
#include "hazel/event/event.h"
#include "hazel/event/mouse_event.h"
#include "hz_pch.h"

#include "editor_layer.h"
#include "imgui.h"
#include <any>
#include <cstdint>
#include <memory>

static std::unordered_map<char, std::array<int, 2>> tile_block_map = {
    {'w', {0, 10}},
    {'R',  {1, 8}},
    {'W', {1, 10}},
    {'G', {5, 10}},
    {'F',  {9, 7}},
    {'f',  {8, 6}},
    {'B', {11, 0}},
};

const int         map_width  = 24;
const int         map_height = 12;
const std::string game_map   = "fFFFFFFFFFFFFFFFFFFFFFFf"
                               "fWWWWWWWWWWWWWWWWWWWWWWf"
                               "fRWGWWWWWWWWWWWWGWWWWWWf"
                               "fRWWWWWBWWWBWWWWWWWWWWWf"
                               "fRWWGWWWWWWWWWWWGWWWWWWf"
                               "fRWWWWWWWWWWWWWWWWWWWWWf"
                               "fRWWGWWWWWWBWWWWWWWWWWWf"
                               "fRWWWWWWBWWWWWWWGWWWWWWf"
                               "fRWWGWWWWWWWWWGGGWWWWWWf"
                               "fRWWWWWWWWWWWWWWWWWWWWWf"
                               "fWWRRRRRRRRRRRRRRRRWWWWf"
                               "fFFFFFFFFFFFFFFFFFFFFFFf";

namespace hazel {

EditorLayer::EditorLayer() : Layer("Editor Layer") {}

void EditorLayer::OnAttach()
{
    Init();
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(Timestep ts)
{
    HZ_PROFILE_SCOPE("Sandbox2d::OnUpdate");

    // TODO: the camera's event is block by imgui_layer, but keybord event still handled by here
    if (bViewPortFocusing) {
        m_CameraController.OnUpdate(ts);
    }

    hazel::Render2D::ResetStats();

    m_Framebuffer->Bind();
    {


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


            // hazel::Render2D::BeginScene(m_CameraController.GetCamera());
            // for (float y = -5.f; y < 5.f; y += 0.5f) {
            //     for (float x = -5.f; x < 5.f; x += 0.5f) {
            //         glm::vec2 rb    = {sin(x), cos(y * x)};
            //         glm::vec4 color = glm::vec4(rb, tan(y), 0.3f);

            //         hazel::Render2D::DrawQuad(glm::vec2{x, y}, {1, 1}, color);
            //     }
            // }
            // hazel::Render2D::EndScene();
        }


        if (hazel::Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_LEFT)) {

            auto [x, y] = hazel::Input::GetMousePos();
            // HZ_INFO("{} {}", x, y);
            auto w = hazel::App::Get().GetWindow().GetWidth();
            auto h = hazel::App::Get().GetWindow().GetHeight();

            auto bounds = m_CameraController.GetBounds();
            auto pos    = m_CameraController.GetCamera().GetPosition();

            x = (x / w) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
            y = bounds.GetHeight() * 0.5f - (y / h) * bounds.GetHeight();

            m_PracticleProps.Position = {x + pos.x, y + pos.y};
            // HZ_INFO("Should be {} {}", x + pos.x, y + pos.y);

            auto i = 10;
            while (i--) {
                m_PracticleSystem.Emit(m_PracticleProps);
            }
        }

        m_PracticleSystem.OnUpdate(ts);
        m_PracticleSystem.OnRender(m_CameraController.GetCamera());
    }
    m_Framebuffer->Unbind();
};

void EditorLayer::OnImGuiRender()
{
    static bool               opt_fullscreen  = true;
    static bool               opt_padding     = true;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    {
        if (opt_fullscreen)
        {
            const ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 1.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

            // for main docspace as root
            window_flags |= ImGuiWindowFlags_NoTitleBar |
                            ImGuiWindowFlags_NoCollapse |
                            ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoMove;

            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus |
                            ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
            window_flags |= ImGuiWindowFlags_NoBackground;
        }

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        }
    }

    ImGui::Begin("MainWindow", NULL, window_flags);
    {
        if (!opt_padding) {
            ImGui::PopStyleVar();
        }

        if (opt_fullscreen) {
            ImGui::PopStyleVar(2);
        }

        // Submit the DockSpace
        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("Main Dock Space");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            // ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
        }

        if (ImGui::BeginMenuBar())
        {

            if (ImGui::BeginMenu("file"))
            {
                if (ImGui::MenuItem("Exit")) {
                    hazel::App::Get().Close();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Options")) {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                ImGui::MenuItem("Padding", NULL, &opt_padding);
                ImGui::Separator();

                if (ImGui::MenuItem("Flag: NoDockingOverCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingOverCentralNode) != 0)) {
                    dockspace_flags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode;
                }
                if (ImGui::MenuItem("Flag: NoDockingSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingSplit) != 0)) {
                    dockspace_flags ^= ImGuiDockNodeFlags_NoDockingSplit;
                }
                if (ImGui::MenuItem("Flag: NoUndocking", "", (dockspace_flags & ImGuiDockNodeFlags_NoUndocking) != 0)) {
                    dockspace_flags ^= ImGuiDockNodeFlags_NoUndocking;
                }
                if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) {
                    dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
                }
                if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) {
                    dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
                }
                if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) {
                    dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
                }
                ImGui::Separator();

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }


        if (ImGui::Begin("Settings")) {
            ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_ClearColor));
            ImGui::DragFloat3("Quad Position", glm::value_ptr(m_QuadPosition));
            ImGui::ColorEdit4("Flat Color", glm::value_ptr(m_FlatColor));
            ImGui::InputFloat2("Shift of tilemap", m_Shiftting);
            auto id = m_ArchTexture->GetTextureID();
            ImGui::Image((void *)id, ImVec2{64, 64});
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


        ViewPort();

        ImGui::End();
    }
}

void EditorLayer::OnEvent(hazel::Event &event)
{
    m_CameraController.OnEvent(event);
}

void EditorLayer::Init()
{
    hazel::FramebufferSpecification spec;
    spec.Width    = 1280;
    spec.Height   = 720;
    m_Framebuffer = hazel::Framebuffer::Create(spec);

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

    // TODO: the margin between 2 blocks as it is a unie texture?
    // solution: use the packed texture....
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

void EditorLayer::ViewPort()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});

    if (ImGui::Begin("ViewPort"))
    {
        {
            bViewPortFocusing = ImGui::IsWindowFocused();
            bViewPortHovering = ImGui::IsWindowHovered();
            // HZ_CORE_WARN("is foucused {}", bViewPortFocusing);
            App::Get().GetImGuiLayer()->SetBlockEvents(!bViewPortFocusing || !bViewPortFocusing);
        }

        ImGui::PopStyleVar();

        const ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();

        const float x = viewport_panel_size.x,
                    y = viewport_panel_size.y;

        if (m_ViewportSize != glm::vec2{x, y})
        {
            HZ_INFO("Viewpor  resized: {} {}", x, y);
            m_ViewportSize = {x, y};

            m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
        }

        uint32_t framebuffer_colorattachment = m_Framebuffer->GetColorAttachmentID();
        ImGui::Image((void *)framebuffer_colorattachment,
                     ImVec2{m_ViewportSize.x, m_ViewportSize.y},
                     ImVec2{0, 1}, ImVec2{1, 0} // flip the image
        );
        ImGui::End();
    }
}

} // namespace hazel