#include "hz_pch.h"

#include "glm/ext/vector_float3.hpp"
#include "hazel/imgui/imgui_layer.h"
#include "hazel/scene/scene_serializer.h"

#include "hazel/core/timestep.h"

#include "glm/ext/vector_float2.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "hazel/core/app.h"
#include "hazel/core/layer.h"
#include "hazel/core/log.h"
#include "hazel/event/event.h"
#include "hazel/renderer/framebuffer.h"
#include "hazel/scene/component.h"


#include "editor_layer.h"
#include "imgui.h"
#include <cstdint>
#include <memory>
#include <objidlbase.h>
#include <string>
#include <sysinfoapi.h>
#include <vector>

#include "utils/path.h"

namespace hazel {

EditorLayer::EditorLayer() : Layer("Editor Layer") {}

EditorLayer::~EditorLayer()
{
    // HZ_INFO("count: {} ", m_TinyTownSheet.use_count());
    // m_TinyTownSheet.reset();
}

void EditorLayer::OnAttach()
{
    Init();
    m_ActiveScene = hazel::CreateRef<hazel::Scene>();
    m_SceneHierachyPanel.SetContext(m_ActiveScene);

    m_CameraEntity = m_ActiveScene->CreateEntity("camera_entity A");
    m_CameraEntity.AddComponent<CameraComponent>();
    m_CameraEntity.GetComponent<TransformComponent>().Translation.z += 5;

    m_SecondCameraEntity                                          = m_ActiveScene->CreateEntity("camera_entity B");
    m_SecondCameraEntity.AddComponent<CameraComponent>().bPrimary = false;
    m_CameraEntity.GetComponent<TransformComponent>().Translation.z += 5;


    m_SquareEntity = m_ActiveScene->CreateEntity("Red Square");
    m_SquareEntity.AddComponent<SpriteRendererComponent>(glm::vec4{1, 0, 0, 1});

    m_GreenSquareEntity = m_ActiveScene->CreateEntity("Green Square");
    m_GreenSquareEntity.AddComponent<SpriteRendererComponent>(glm::vec4{0, 1, 0, 1});
    m_GreenSquareEntity.GetComponent<TransformComponent>().Translation += 1;



    class CameraController : public ScriptableEntity
    {
      public:
        void OnCreate()
        {
            HZ_CORE_INFO("{}", __FUNCSIG__);
        }
        void OnDestory() {}
        void OnUpdate(Timestep ts)
        {
            // HZ_CORE_INFO("Timestep: {}", ts.GetSeconds());
            auto &translation = GetComponent<TransformComponent>().Translation;
            float speed       = 5.f;
            if (hazel::Input::IsKeyPressed(HZ_KEY_A))
                translation[0] -= speed * ts;
            if (hazel::Input::IsKeyPressed(HZ_KEY_D))
                translation[0] += speed * ts;
            if (hazel::Input::IsKeyPressed(HZ_KEY_W))
                translation[1] -= speed * ts;
            if (hazel::Input::IsKeyPressed(HZ_KEY_S))
                translation[1] += speed * ts;
        }
    };

    m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(Timestep ts)
{
    HZ_PROFILE_SCOPE("Sandbox2d::OnUpdate");

    if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
        m_ViewportSize.x > 0.f && m_ViewportSize.y > 0.f &&
        (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
    {
        m_Framebuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);
        m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
        // here set the ortho every fame of camera
        m_ActiveScene->OnViewportResize(m_ViewportSize.x, m_ViewportSize.y);
    }

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
    }


    m_ActiveScene->OnUpdate(ts);

    m_Framebuffer->Unbind();
};


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
}

void EditorLayer::OnImGuiRender()
{
    UpdateWindowFlags();

    ImGui::Begin("MainWindow", NULL, m_WindowFlags);
    {
        if (!bPadding) {
            ImGui::PopStyleVar();
        }

        if (bFullscreen) {
            ImGui::PopStyleVar(2);
        }

        // Submit the DockSpace
        ImGuiIO &io = ImGui::GetIO();

        ImGuiStyle &style = ImGui::GetStyle();
        {
            float min_windows_width = style.WindowMinSize.x;
            style.WindowMinSize.x   = 320.f;

            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGuiID dockspace_id = ImGui::GetID("Main Dock Space");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), m_DockspaceFlags);
                // ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
            }

            style.WindowMinSize.x = min_windows_width;
        }

        MenuBar();


        m_SceneHierachyPanel.OnImGuiRender();


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
        if (ImGui::Begin("Settings")) {
            ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_ClearColor));
            auto id = m_ArchTexture->GetTextureID();
            ImGui::Image((void *)id, ImVec2{64, 64});

            FontSwitcher();

            ImGui::End();
        }



        ViewPort();

        ImGui::End();
    }
} // namespace hazel


void EditorLayer::UpdateWindowFlags()
{
    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    // ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (bFullscreen)
    {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 1.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        // for main docspace as root
        m_WindowFlags |= ImGuiWindowFlags_NoTitleBar |
                         ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove;

        m_WindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus |
                         ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        m_DockspaceFlags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (m_DockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode) {
        m_WindowFlags |= ImGuiWindowFlags_NoBackground;
    }

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    if (!bPadding)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    }
}

void EditorLayer::MenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("file"))
        {

            if (ImGui::MenuItem("Serilize")) {
                SceneSerializer Serialize(m_ActiveScene);
                Serialize.Serialize(FPath("tmp/a.yaml"));
            }
            if (ImGui::MenuItem("Deserilize")) {
                SceneSerializer Serialize(m_ActiveScene);
                Serialize.Deserialize(FPath("tmp/a.yaml"));
            }

            if (ImGui::MenuItem("Exit")) {
                hazel::App::Get().Close();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Options")) {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.
            ImGui::MenuItem("Fullscreen", NULL, &bFullscreen);
            ImGui::MenuItem("Padding", NULL, &bPadding);
            ImGui::Separator();

            if (ImGui::MenuItem("Flag: NoDockingOverCentralNode", "", (m_DockspaceFlags & ImGuiDockNodeFlags_NoDockingOverCentralNode) != 0)) {
                m_DockspaceFlags ^= ImGuiDockNodeFlags_NoDockingOverCentralNode;
            }
            if (ImGui::MenuItem("Flag: NoDockingSplit", "", (m_DockspaceFlags & ImGuiDockNodeFlags_NoDockingSplit) != 0)) {
                m_DockspaceFlags ^= ImGuiDockNodeFlags_NoDockingSplit;
            }
            if (ImGui::MenuItem("Flag: NoUndocking", "", (m_DockspaceFlags & ImGuiDockNodeFlags_NoUndocking) != 0)) {
                m_DockspaceFlags ^= ImGuiDockNodeFlags_NoUndocking;
            }
            if (ImGui::MenuItem("Flag: NoResize", "", (m_DockspaceFlags & ImGuiDockNodeFlags_NoResize) != 0)) {
                m_DockspaceFlags ^= ImGuiDockNodeFlags_NoResize;
            }
            if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (m_DockspaceFlags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) {
                m_DockspaceFlags ^= ImGuiDockNodeFlags_AutoHideTabBar;
            }
            if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (m_DockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, bFullscreen)) {
                m_DockspaceFlags ^= ImGuiDockNodeFlags_PassthruCentralNode;
            }
            ImGui::Separator();

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
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

        if (m_ViewportSize != glm::vec2{x, y} && x > 0 && y > 0)
        {
            HZ_INFO("Viewpor  resized: {} {}", x, y);
            m_ViewportSize = {x, y};
        }

        uint32_t framebuffer_colorattachment = m_Framebuffer->GetColorAttachmentID();
        ImGui::Image((void *)framebuffer_colorattachment,
                     ImVec2{m_ViewportSize.x, m_ViewportSize.y},
                     ImVec2{0, 1}, ImVec2{1, 0} // flip the image
        );
        ImGui::End();
    }
}

void EditorLayer::FontSwitcher()
{
    auto &io    = ImGui::GetIO();
    auto  fonts = App::Get().GetImGuiLayer()->GetFonts();

    static int                      current_font_index = 0;
    static std::vector<std::string> keys;
    static std::vector<ImFont *>    values;

    if (keys.size() != fonts.size()) {
        int i = 0;
        keys.clear();
        values.clear();
        for (const auto &[name, font_ptr] : fonts) {
            if (io.FontDefault == font_ptr) {
                current_font_index = i;
            }
            keys.push_back(name);
            values.push_back(font_ptr);
            ++i;
        }
    }

    if (ImGui::BeginCombo("Font", keys[current_font_index].c_str()))
    {
        for (int i = 0; i < keys.size(); ++i) {
            bool bSelected = current_font_index == i;

            if (ImGui::Selectable(keys[i].c_str(), bSelected)) {
                current_font_index = i;
                io.FontDefault     = values[i];
            }
            if (bSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
}

} // namespace hazel
