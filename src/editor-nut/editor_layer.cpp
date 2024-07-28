#include "hz_pch.h"
//

#include "hazel/core/mouse_button.h"
#include "hazel/scene/editor_camera.h"

#include "glm/fwd.hpp"

#include "hazel/core/base.h"
#include "hazel/core/input.h"
#include "hazel/core/key_code.h"
#include "hazel/event/key_event.h"
#include "hazel/scene/scene.h"
#include "hazel/utils/platform_utils.h"


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

#include <imgui.h>

#include "ImGuizmo.h"
#include "editor_layer.h"
#include <cstdint>
#include <objidlbase.h>
#include <string>
#include <sysinfoapi.h>



#include "math/math.h"
#include "utils/path.h"

namespace hazel {

EditorLayer::EditorLayer() : Layer("Editor Layer")
{
}

EditorLayer::~EditorLayer()
{
    // HZ_INFO("count: {} ", m_TinyTownSheet.use_count());
    // m_TinyTownSheet.reset();
}

void EditorLayer::OnAttach()
{
    hazel::FramebufferSpec spec;
    spec.Attachments = {framebuffer::ETextureFormat::RGBA8, framebuffer::ETextureFormat::RED_INTEGER, framebuffer::ETextureFormat::Depth};
    spec.Width       = 800;
    spec.Height      = 600;
    m_Framebuffer    = hazel::Framebuffer::Create(spec);

    // m_FaceTexture  = hazel::Texture2D::Create(FPath("res/texture/face.png"));
    // m_ArchTexture  = hazel::Texture2D::Create(FPath("res/texture/arch.png"));
    // m_BlockTexture = hazel::Texture2D::Create(FPath("res/texture/block.png"));
    m_SceneHierarchyPanel.SetContext(m_ActiveScene);
    m_EditorCamera = EditorCamera(30.f, 1.6 / 0.9, 0.1, 1000.0);

    m_ActiveScene = hazel::CreateRef<hazel::Scene>();

    auto commandlines = App::Get().GetCommandLineArgs();
    if (commandlines.count > 1) {
        auto            scene_filepath = commandlines.args[1];
        SceneSerializer serializer(m_ActiveScene);
        serializer.Deserialize(scene_filepath);
    }



#if 0
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
        void OnDestroy() {}
        void OnUpdate(Timestep ts)
        {
            // TODO: Disable this on:
            // 1. Select a entity in editor
            // 2. Start playing games
            // HZ_CORE_INFO("Timestep: {}", ts.GetSeconds());
            auto &translation = GetComponent<TransformComponent>().Translation;
            float speed       = 5.f;
            if (hazel::Input::IsKeyPressed(Key::A))
                translation[0] -= speed * ts;
            if (hazel::Input::IsKeyPressed(Key::D))
                translation[0] += speed * ts;
            if (hazel::Input::IsKeyPressed(Key::W))
                translation[1] -= speed * ts;
            if (hazel::Input::IsKeyPressed(Key::S))
                translation[1] += speed * ts;
        }
    };

    m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
#endif
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(Timestep ts)
{
    HZ_PROFILE_SCOPE("Sandbox2d::OnUpdate");

    m_ActiveScene->OnViewportResize(m_ViewportSize.x, m_ViewportSize.y);

    // resize
    if (const FramebufferSpec &spec = m_Framebuffer->GetSpecification();
        m_ViewportSize.x > 0.f && m_ViewportSize.y > 0.f &&
        (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
    {
        m_Framebuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);
        m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
        // here set the ortho every fame of camera
        m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
    }

    // TODO: the camera's event is block by imgui_layer, but keyboard event still handled by here
    if (bViewPortFocusing) {
        m_CameraController.OnUpdate(ts);
    }
    m_EditorCamera.OnUpdate(ts);

    hazel::Render2D::ResetStats();

    m_Framebuffer->Bind();
    {
        HZ_PROFILE_SCOPE("Renderer Prep");
        hazel::RenderCommand::SetClearColor(m_ClearColor);
        hazel::RenderCommand::Clear();

        // reset the entity_id attachment to -1
        m_Framebuffer->ClearAttachment(1, -1);
    }

    m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);

    // Mouse hover/picking preps
    {
        auto [mx, my] = ImGui::GetMousePos();
        mx -= m_ViewportBounds[0].x;
        my -= m_ViewportBounds[0].y;
        my = m_ViewportSize.y - my; // flip y, make 0,0 bot-left

        if (mx >= 0 && my >= 0 && mx < (int)m_ViewportSize.x && my < (int)m_ViewportSize.y)
        {
            int pixel       = m_Framebuffer->ReadPixel(1, mx, my);
            m_HoveredEntity = pixel == -1 ? Entity{} : Entity{(entt::entity)pixel, m_ActiveScene.get()};
            // HZ_CORE_WARN("Pos in view port: {},{}, pixel= {}", mx, my, pix);
        }
    }

    m_Framebuffer->Unbind();
};


void EditorLayer::OnEvent(hazel::Event &event)
{
    static bool bPlaying = false;
    if (!m_SceneHierarchyPanel.GetSelectedEntity()) {
        m_CameraController.OnEvent(event);
    }
    m_EditorCamera.OnEvent(event);


    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(HZ_BIND_EVENT(this, &EditorLayer::OnKeyPressed));
    dispatcher.Dispatch<MouseButtonPressedEvent>(HZ_BIND_EVENT(this, &EditorLayer::OnMouseButtonPressed));
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
            style.WindowMinSize.x   = min_windows_width;
        }

        //  make this window dockspace
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("Main Dock Space");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), m_DockspaceFlags);
            // ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
        }

        MenuBar();


        m_SceneHierarchyPanel.OnImGuiRender();


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
            auto pos = m_EditorCamera.GetPosition();
            ImGui::Text("Postion: %f, %f, %f", pos.x, pos.y, pos.z);
            ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_ClearColor));
            // auto id = m_ArchTexture->GetTextureID();
            // ImGui::Image(reinterpret_cast<void *>(id), ImVec2{64, 64});
            ImGui::InputInt("Attachment Id", &m_ViewportColorAttachmentId);

            const char *name = "None";
            if (m_HoveredEntity) {
                name = m_HoveredEntity.GetComponent<TagComponent>().Tag.c_str();
            }
            ImGui::Text("Hovered Entity: %s", name);

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

        // for main dockspace as root
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
    // all active windows docked into it will lose their parent and become unlocked.
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
            // TODO: MOVE short cuts info configuration file
            if (ImGui::MenuItem("New")) {
                NewScene();
            }
            if (ImGui::MenuItem("Open...", "Ctrl+O")) {
                OpenScene();
            }
            if (ImGui::MenuItem("Save as...", "Ctrl+Shift+S")) {
                SaveAs();
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
            auto viewport_offset     = ImGui::GetWindowPos(); // include tab bar
            auto viewport_min_region = ImGui::GetWindowContentRegionMin();
            auto viewport_max_region = ImGui::GetWindowContentRegionMax();
            m_ViewportBounds[0]      = {viewport_min_region.x + viewport_offset.x, viewport_min_region.y + viewport_offset.y};
            m_ViewportBounds[1]      = {viewport_max_region.x + viewport_offset.x, viewport_max_region.y + viewport_offset.y};

            bViewPortFocusing = ImGui::IsWindowFocused();
            bViewPortHovering = ImGui::IsWindowHovered();
            // HZ_CORE_WARN("is focused {}", bViewPortFocusing);
            App::Get().GetImGuiLayer()->SetBlockEvents(!bViewPortFocusing && !bViewPortFocusing);
        }

        ImGui::PopStyleVar();

        const ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
        const float  x                   = viewport_panel_size.x;
        const float  y                   = viewport_panel_size.y;

        if (m_ViewportSize != glm::vec2{x, y} && x > 0 && y > 0)
        {
            HZ_INFO("Viewport  resized: {} {}", x, y);
            m_ViewportSize = {x, y};
        }

        uint64_t framebuffer_colorattachment = m_Framebuffer->GetColorAttachmentID(m_ViewportColorAttachmentId);
        ImGui::Image(reinterpret_cast<void *>(framebuffer_colorattachment),
                     ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2{0, 1}, ImVec2{1, 0} // flip the image
        );


        Gizmos();

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


void EditorLayer::Gizmos()
{
    Entity selected_entity = m_SceneHierarchyPanel.GetSelectedEntity();
    if (!selected_entity || m_GizmoType == -1) {
        return;
    }

    // ImGuizmo::Enable(true);

    ImGuizmo::SetDrawlist();

    ImGuizmo::SetOrthographic(false);

    ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y,
                      m_ViewportBounds[1].x - m_ViewportBounds[0].x,
                      m_ViewportBounds[1].y - m_ViewportBounds[0].y);

    // runtime camera from entity
    // auto ce = m_ActiveScene->GetPrimaryCameraEntity();
    // HZ_ASSERT(ce, "Should be valid");
    // const SceneCamera &camera            = ce.GetComponent<CameraComponent>().Camera;
    // const glm::mat4   &camera_projection = camera.GetProjection();
    // const glm::mat4    camera_view       = glm::inverse(ce.GetComponent<TransformComponent>().GetTransform());

    // Editor camera
    const glm::mat4 &camera_projection = m_EditorCamera.GetProjection();
    const glm::mat4  camera_view       = m_EditorCamera.GetViewMatrix();

    // selected entity transform
    auto     &tc        = selected_entity.GetComponent<TransformComponent>();
    glm::mat4 transform = tc.GetTransform();


    // do transform a section by a section
    bool  snap       = Input::IsKeyPressed(Key::LeftControl);
    float snap_value = 0.5f;
    if (m_GizmoType == ImGuizmo::OPERATION::ROTATE) {
        snap_value = 45.f;
    }
    float snap_values[3] = {
        snap_value,
        snap_value,
        snap_value,
    };

    ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection),
                         ImGuizmo::OPERATION(m_GizmoType), ImGuizmo::MODE::LOCAL,
                         glm::value_ptr(transform),
                         nullptr, snap ? snap_values : nullptr);

    if (ImGuizmo::IsUsing())
    {
        glm::vec3 translation, rotation, scale;
        math::DecomposeTransform(transform, translation, rotation, scale);
        // ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), glm::value_ptr(translation),
        //                                       glm::value_ptr(rotation), glm::value_ptr(scale));

        tc.Translation = translation;
        tc.Scale       = scale;

        // tc.Rotation    = rotation;
        glm::vec3 rotation_delta = rotation - tc.Rotation;
        tc.Rotation += rotation_delta;
    }
}

bool EditorLayer::OnKeyPressed(const KeyPressedEvent &Ev)
{
    if (Ev.GetRepeatCount() > 0) {
        return false;
    }
    bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
    bool shift   = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
    if (!ImGuizmo::IsUsing()) {
        switch (Ev.GetKeyCode()) {
            case Key::N:
            {
                if (control) {
                    NewScene();
                }
                break;
            }
            case Key::O:
            {
                if (control) {
                    OpenScene();
                }
                break;
            }
            case Key::S:
            {
                if (control && shift) {
                    SaveAs();
                }
                break;
            }
            default:
                break;
        }
    }

    auto selected_entity = m_SceneHierarchyPanel.GetSelectedEntity();

    if (selected_entity) {
        switch (Ev.GetKeyCode()) {
            // Gizmos
            case Key::Q:
            {
                m_GizmoType = -1;
                break;
            }
            case Key::W:
            {
                m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
                break;
            }
            case Key::E:
            {
                m_GizmoType = ImGuizmo::OPERATION::ROTATE;
                break;
            }
            case Key::R:
            {
                m_GizmoType = ImGuizmo::OPERATION::SCALE;
                break;
            }
        }
        return false;
    }

    return false;
}

bool EditorLayer::OnMouseButtonPressed(const MouseButtonPressedEvent &Ev)
{
    Mouse::Button btn = (Mouse::Button)Ev.GetMouseButton();
    switch (btn) {
        case Mouse::Button_Left:
        {
            if (bViewPortHovering && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt)) {
                m_SceneHierarchyPanel.SetSelection(m_HoveredEntity);
            }
            break;
        }
    }
    return false;
}

void EditorLayer::NewScene()
{
    m_ActiveScene = CreateRef<Scene>(); // Just create a new scene/new tab(TODO)
    m_ActiveScene->OnViewportResize(m_ViewportSize.x, m_ViewportSize.y);
    m_SceneHierarchyPanel.SetContext(m_ActiveScene);
}

void EditorLayer::OpenScene()
{
    auto path = FileDialogs::OpenFile("Hazel Scene (*.hazel)\0*.hazel\0"
                                      "Hazel Scene (*.yaml)\0*.yaml\0");
    if (!path.empty()) {
        m_ActiveScene = CreateRef<Scene>(); // Just create a new scene/new tab(TODO)
        m_ActiveScene->OnViewportResize(m_ViewportSize.x, m_ViewportSize.y);
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);
        SceneSerializer Serialize(m_ActiveScene);
        Serialize.Deserialize(path);
    }
}

void EditorLayer::SaveAs()
{
    auto path = FileDialogs::SaveFile("Hazel Scene (*.hazel)\0*.hazel\0"
                                      "Hazel Scene (*.yaml)\0*.yaml\0");
    if (!path.empty()) {
        if (!FPath(path.c_str()).absolute_path.has_extension()) {
            path.append(".hazel");
        }
        SceneSerializer Serialize(m_ActiveScene);
        Serialize.Serialize(path);
    }
}

} // namespace hazel
