/**
 *  Author: @godot42
 *  Create Time: 2024-07-28 20:32:18
 * @ Modified by: @godot42
 * @ Modified by: @godot42
 * @ Modified time: 2024-12-29 01:02:18
 */

#include "glm/ext/quaternion_geometric.hpp"
#include "hz_pch.h"
//

#include "glm/matrix.hpp"
#include "hazel/scene/entity.h"
#include "hazel/scene/scene.h"
//
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "hazel/renderer/render_2d.h"
//

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

#include <cstdio>
#include <filesystem>
#include <imgui.h>

#include "ImGuizmo.h"
#include "editor_layer.h"
#include <cstdint>
#include <objidlbase.h>
#include <processthreadsapi.h>
#include <string>
#include <sysinfoapi.h>
#include <vector>



#include "math/math.h"
#include "utils/path.h"


// debug
#include "platform/opengl/gl_macros.h"
#include <gl/GL.h>
#include <glad/glad.h>
#include <winuser.h>

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
    if (bDefaultMaximizeWindow) {
        App::Get().GetWindow().SetMaximized();
    }

    m_IconPlay     = Texture2D::Create(FPath("res/texture/editor/play.png"));
    m_IconStop     = Texture2D::Create(FPath("res/texture/editor/stop.png"));
    m_IconSimulate = Texture2D::Create(FPath("res/texture/editor/simulate_button.png"));
    // m_FaceTexture  = hazel::Texture2D::Create(FPath("res/texture/face.png"));
    // m_ArchTexture  = hazel::Texture2D::Create(FPath("res/texture/arch.png"));
    // m_BlockTexture = hazel::Texture2D::Create(FPath("res/texture/block.png"));


    hazel::FramebufferSpec spec;
    spec.Attachments = {framebuffer::ETextureFormat::RGBA8,
                        framebuffer::ETextureFormat::RED_INTEGER, // EntityId
                        framebuffer::ETextureFormat::Depth};
    spec.Width       = 800;
    spec.Height      = 600;
    m_Framebuffer    = hazel::Framebuffer::Create(spec);

    m_EditorScene = hazel::CreateRef<hazel::Scene>();
    SetActiveScene(m_EditorScene);


    m_EditorCamera = EditorCamera(45.f, 1.6 / 0.9, 0.1, 1000.0);

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

    m_SceneHierarchyPanel.SetContext(m_ActiveScene);
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


    hazel::Render2D::ResetStats();

    m_Framebuffer->Bind();
    {
        HZ_PROFILE_SCOPE("Renderer Prep");
        hazel::RenderCommand::SetClearColor(m_ClearColor);
        hazel::RenderCommand::Clear();

        // reset the entity_id attachment to -1
        m_Framebuffer->ClearAttachment(1, -1);
    }

    // draw scene in editor
    switch (m_SceneState) {

        case ESceneState::Stop:
        {
            // TODO: the camera's event is block by imgui_layer, but keyboard event still handled by here
            if (bViewPortFocusing) {
                m_CameraController.OnUpdate(ts);
            }
            m_EditorCamera.OnUpdate(ts);

            m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
            break;
        }
        case ESceneState::Play:
        {
            m_ActiveScene->OnUpdateRuntime(ts);
            break;
        }
        case ESceneState::Simulate:
        {
            // Why not update editor camera controller?
            m_EditorCamera.OnUpdate(ts);
            m_ActiveScene->OnUpdateSimulation(ts, m_EditorCamera);
            break;
        }
    }

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



    OnOverlayRender();
    OnDebugRender();

    m_Framebuffer->Unbind();
};


void EditorLayer::OnEvent(hazel::Event &event)
{
    static bool bPlaying = false;
    if (!m_SceneHierarchyPanel.GetSelectedEntity()) {
        m_CameraController.OnEvent(event);
    }
    if (m_SceneState == ESceneState::Stop) {
        m_EditorCamera.OnEvent(event);
    }


    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<KeyPressedEvent>(HZ_BIND_EVENT(this, &EditorLayer::OnKeyPressed));
    dispatcher.Dispatch<MouseButtonPressedEvent>(HZ_BIND_EVENT(this, &EditorLayer::OnMouseButtonPressed));
}

std::filesystem::path EditorLayer::DefaultAssetsDirectory()
{
    static std::filesystem::path s_AssetsDirectory = FPath("res");
    return s_AssetsDirectory;
}

void EditorLayer::SetActiveScene(Ref<Scene> new_scene)
{
    // m_RuntimeScene = nullptr; // destroy by assign
    m_ActiveScene = new_scene;
    m_SceneHierarchyPanel.SetContext(m_ActiveScene);
}

void EditorLayer::OnOverlayRender()
{
    if (!bShow2DPhysicsCollisions) {
        return;
    }

    if (m_SceneState == ESceneState::Play)
    {
        Entity camera = m_ActiveScene->GetPrimaryCameraEntity();
        if (!camera) {
            return;
        }
        Render2D::BeginScene(camera.GetComponent<CameraComponent>().Camera,
                             camera.GetComponent<TransformComponent>().GetTransform());
    }
    else {
        Render2D::BeginScene(m_EditorCamera);
    }

    // Box
    {
        auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
        for (auto entity : view) {
            const auto &[tc, bc2dc] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

            glm::vec3 pos    = tc.Translation + glm::vec3(bc2dc.Offset, 0.001f);
            glm::vec3 scale  = tc.Scale * glm::vec3(bc2dc.Size * 2.0f, 1.f);
            glm::mat4 transf = glm::translate(glm::mat4(1.0f), pos) *
                               glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0, 0, 1)) *
                               glm::scale(glm::mat4(1.0f), scale);

            Render2D::DrawRect(transf, m_DebugCollisionColor);
        }
    }



    // Circle
    {
        auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
        for (auto entity : view) {
            const auto &[tc, cc2dc] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

            glm::vec3 pos    = tc.Translation + glm::vec3(cc2dc.Offset, 0.001f);
            glm::vec3 scale  = tc.Scale * (cc2dc.Radius * 2);
            glm::mat4 transf = glm::translate(glm::mat4(1.0f), pos) *
                               glm::scale(glm::mat4(1.0f), scale);

            Render2D::DrawCircle(transf, m_DebugCollisionColor, 0.05f);
        }
    }


    // Draw selected entity outline
    if (Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity()) {
        const TransformComponent &tc = selectedEntity.GetComponent<TransformComponent>();
        // Red
        auto color  = glm::vec4(1, 0, 0, 1);
        auto transf = glm::translate(tc.GetTransform(), glm::vec3(0, 0, 0.1f));

        Render2D::DrawRect(transf, color);
    }

    Render2D::EndScene();
    GL_CHECK_HEALTH();
}
void EditorLayer::OnDebugRender()
{
}


void EditorLayer::OnImGuiRender()
{
    UpdateWindowFlags();

    ImGui::Begin("MainWindow", NULL, m_WindowFlags);

    if (bFullscreen) {
        ImGui::PopStyleVar(2);
    }
    if (!bPadding) {
        ImGui::PopStyleVar();
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
    UI_Toolbar();
    UI_Settings();
    Win_RenderStats();

    m_ContentBrowserPanel.OnImGuiRender();
    m_SceneHierarchyPanel.OnImGuiRender();

    ViewPort();


    ImGui::End();
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
    ImGui::BeginMenuBar();

    if (ImGui::BeginMenu("file"))
    {
        // TODO: MOVE short cuts info configuration file
        if (ImGui::MenuItem("New")) {
            NewScene();
        }
        if (ImGui::MenuItem("Open...", "Ctrl+O")) {
            OpenScene();
        }
        if (ImGui::MenuItem("Save", "Ctrl+S")) {
            SaveScene();
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
    if (ImGui::BeginMenu("Editor")) {
        if (ImGui::MenuItem("Settings", "", bShowSettingsWindow)) {
            bShowSettingsWindow = !bShowSettingsWindow;
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Windows")) {
        if (ImGui::MenuItem("Render Stats", "", bShowRenderStats))
        {
            bShowRenderStats = !bShowRenderStats;
        }
        ImGui::EndMenu();
    }

    ImGui::EndMenuBar();
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
        ImGui::Image((void *)(framebuffer_colorattachment),
                     ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2{0, 1}, ImVec2{1, 0} // flip the image
        );


        if (ImGui::BeginDragDropTarget())
        {
            // from content browser's item
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                if (payload->Data) {
                    const wchar_t        *path = (const wchar_t *)payload->Data;
                    std::filesystem::path p(path);
                    HZ_CORE_INFO("{} : {}", p.is_absolute() ? "absolute" : "relative", p.string());

                    auto filename = p.filename().string();

                    if (filename.ends_with(".hazel"))
                    {
                        if (p.is_relative()) {
                            OpenScene(EditorLayer::DefaultAssetsDirectory() / p);
                        }
                        else {
                            OpenScene(p);
                        }
                    }
                }
                else {
                    HZ_CORE_WARN("No data in payload");
                }
            }

            ImGui::EndDragDropTarget();
        }


        RenderGizmos();

        ImGui::End();
    }
}

void EditorLayer::UI_FontSwitcher()
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

void EditorLayer::UI_Toolbar()
{

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));

    auto &colors = ImGui::GetStyle().Colors;
    ImGui::PushStyleColor(ImGuiCol_Button, {0.5, 0.5, 0.5, 0.3});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.8, 0.6, 0.6, 0.5});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, {0.6, 0.6, 0.6, 0.5});

    if (!ImGui::Begin("##toolbar", nullptr,
                      ImGuiWindowFlags_NoTitleBar |
                          ImGuiWindowFlags_NoDecoration |
                          ImGuiWindowFlags_NoScrollbar |
                          ImGuiWindowFlags_NoScrollWithMouse |
                          ImGuiWindowFlags_NoResize))
    {
        ImGui::End();
        return;
    }

    bool   bToolbarEnable = (bool)m_ActiveScene;
    ImVec4 tint_color     = {1.f, 1.f, 1.f, 1.f};
    if (!bToolbarEnable) {
        tint_color.w = 0.5f;
    }
    float size = ImGui::GetWindowHeight() - 4.0f;


    // play button
    {
        Ref<Texture2D> icon = (m_SceneState == ESceneState::Stop || m_SceneState == ESceneState::Simulate) ? m_IconPlay : m_IconStop;
        ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) -
                             (size * 0.5f) * 1.8f); // 2 button
        bool bPressed = ImGui::ImageButton("Toolbar_Play",
                                           reinterpret_cast<ImTextureID>(icon->GetTextureID()),
                                           ImVec2(size, size),
                                           ImVec2(0, 0), ImVec2(1, 1),
                                           ImVec4(0.0f, 0.0f, 0.0f, 0.0f),
                                           tint_color);

        if (bPressed && bToolbarEnable)
        {
            if (m_SceneState == ESceneState::Stop || m_SceneState == ESceneState::Simulate) {
                OnScenePlay();
            }
            else if (m_SceneState == ESceneState::Play) {
                OnSceneStop();
            }
        }
    }
    ImGui::SameLine();
    // simulate button
    {
        Ref<Texture2D> icon = (m_SceneState == ESceneState::Stop || m_SceneState == ESceneState::Play) ? m_IconSimulate : m_IconStop;
        // ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
        bool bPressed = ImGui::ImageButton("Toolbar_Simulate",
                                           reinterpret_cast<ImTextureID>(icon->GetTextureID()),
                                           ImVec2(size, size),
                                           ImVec2(0, 0), ImVec2(1, 1),
                                           ImVec4(0.0f, 0.0f, 0.0f, 0.0f),
                                           tint_color);
        if (bPressed && bToolbarEnable)
        {
            if (m_SceneState == ESceneState::Stop || m_SceneState == ESceneState::Play) {
                OnSceneSimulate();
            }
            else if (m_SceneState == ESceneState::Simulate) {
                OnSceneStop();
            }
        }
    }

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(3);


    ImGui::End();
}


/* TODO: the ImGui could not load font between begin_frame and end_frame
void EditorLayer::FontSwitcher()
{
    auto *imgui_layer = App::Get().GetImGuiLayer();
    auto  manager     = App::Get().GetImGuiLayer()->GetFontManager();
    static auto scan_fonts = []() {
        struct FontResources {
            std::vector<std::filesystem::path> font_paths;
            std::string                        packed_string;
        } font_resources;

        utils::Files::ForeachFileInFolder(
            FPath("res/font/").absolute_path,
            [](auto &path) -> bool {
                return path.string().ends_with(".ttf");
            },
            [&font_resources](auto &path) -> void {
                font_resources.font_paths.push_back(path.string());
                font_resources.packed_string += (path.stem().string() + '\0');
            });

        font_resources.packed_string += ("ImGuiDefault\0");
        font_resources.font_paths.push_back("None");
        return font_resources;
    };


    static auto font_resources = scan_fonts();
    static int  font_idx       = font_resources.font_paths.size() - 1;

    static auto font_size_options = []() {
        struct {
            std::vector<std::string> pxs;
            std::string              packed_string;

            float get_px(int idx) const
            {
                std::string_view sv = pxs[idx];
                while (sv.back() > '9' && sv.back() < '0') {
                    sv.remove_suffix(1);
                }
                return std::stof(std::string(sv));
            }
        } ret;

        float start = 8.0f;
        for (float i = 0; i < 15; i++) {
            auto opt = fmt::format("{}px", (start + i * 0.5f));
            ret.pxs.push_back(opt);
            ret.packed_string += (opt + '\0');
        }
        return ret;
    }();



    static int font_size_idx = 0;
    if (ImGui::Combo("font", &font_idx, font_resources.packed_string.c_str(), font_resources.font_paths.size())) {
        manager.ChangeFont({
            .name      = font_resources.font_paths[font_idx].stem().string(),
            .pixel_size      = 16.0f,
            .file_path = font_resources.font_paths[font_idx],
        });
    }

    ImGui::SameLine();

    if (ImGui::Combo("font size", &font_size_idx, font_size_options.packed_string.c_str(), font_size_options.pxs.size())) {
        printf("Selected size: %s\n", font_size_options.pxs[font_size_idx].c_str());

        manager.ChangeFont(FontSpec{
            .name      = font_resources.font_paths[font_idx].stem().string(),
            .pixel_size      = font_size_options.get_px(font_size_idx),
            .file_path = font_resources.font_paths[font_idx],
        });
    }

    ImGui::SameLine();

    if (ImGui::Button("Rescan Fonts"))
    {
        font_resources = scan_fonts();
    }
}
*/



void EditorLayer::UI_Settings()
{
    // static bool bWantToClose = false;
    if (!ImGui::Begin("Settings", &bShowSettingsWindow))
    {
        ImGui::End();
        return;
    }
    ImGui::Checkbox("Show 2D Physics Collision", &bShow2DPhysicsCollisions);
    ImGui::ColorEdit4("Debug Collision Color", glm::value_ptr(m_DebugCollisionColor));

    auto pos = m_EditorCamera.GetPosition();
    ImGui::Text("Postion: %f, %f, %f", pos.x, pos.y, pos.z);
    ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_ClearColor));
    // auto id = m_ArchTexture->GetTextureID();
    // ImGui::Image(reinterpret_cast<void *>(id), ImVec2{64, 64});
    ImGui::InputInt("Attachment Id", &m_ViewportColorAttachmentId);

    ImGui::DragFloat("Debug Float", &m_DebugFloat);

    const char *name = "None";
    if (m_HoveredEntity) {
        name = m_HoveredEntity.GetComponent<TagComponent>().Tag.c_str();
    }
    ImGui::Text("Hovered Entity: %s", name);

    UI_FontSwitcher();

    if (ImGui::Button("Reset Editor Camera")) {
        m_EditorCamera = EditorCamera{};
        m_EditorCamera = EditorCamera(30.f, 1.6 / 0.9, 0.1, 1000.0);
    }

    if (ImGui::Button("Duplicate Selected Entity")) {
        m_ActiveScene->DuplicateEntity(m_SceneHierarchyPanel.GetSelectedEntity());
    }

    ImGui::End();
}

void EditorLayer::Win_RenderStats()
{
    if (!bShowRenderStats) {
        return;
    }

    if (!ImGui::Begin("Render2D stats", &bShowRenderStats)) {
        ImGui::End();
        return;
    }
    auto stat = hazel::Render2D::GetStatics();
    ImGui::Text("Draw Calls  : %d", stat.DrawCalls);
    ImGui::Text("Quad Count  : %d", stat.QuadCount);
    ImGui::Text("Vertex Count: %d", stat.GetTotalVertexCount());
    ImGui::Text("Index Count : %d", stat.GetTotalIndexCount());
    auto [x, y] = hazel::Input::GetMousePos();
    ImGui::Text("Mouse Pos : %d,%d", (int)x, (int)y);
    ImGui::End();
}


void EditorLayer::RenderGizmos()
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


    // selected entity transform
    auto     &selected_entity_tc = selected_entity.GetComponent<TransformComponent>();
    glm::mat4 transform          = selected_entity_tc.GetTransform();



    // Get transform from editor/runtime camera
    glm::mat4 camera_projection(1);
    glm::mat4 camera_view(1);
    switch (m_SceneState)
    {
        case ESceneState::Stop:
        case ESceneState::Simulate:
            camera_projection = m_EditorCamera.GetProjection();
            camera_view       = m_EditorCamera.GetViewMatrix();
            break;
        case ESceneState::Play:
        {

            Entity const *ce = m_ActiveScene->m_RuntimeCameraEntity;
            if (!ENSURE(ce)) {
                break;
            }
            camera_projection = ce->GetComponent<CameraComponent>().Camera.GetProjection();

            const auto &tfc = ce->GetComponent<TransformComponent>();

            glm::mat4  rotation = glm::toMat4(glm::quat(glm::vec3(-tfc.Rotation.x, -tfc.Rotation.y, 0.0f)));
            const auto view     = glm::translate(glm::mat4(1.f), tfc.Translation) * rotation;

            camera_view = glm::inverse(view);
            break;
        }
        default:
            HZ_ASSERT(false, "Unknown scene state");
            break;
    }



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

        selected_entity_tc.Translation = translation;
        selected_entity_tc.Scale       = scale;

        // tc.Rotation    = rotation;
        glm::vec3 rotation_delta = rotation - selected_entity_tc.Rotation;
        selected_entity_tc.Rotation += rotation_delta;
    }
}

bool EditorLayer::OnKeyPressed(const KeyPressedEvent &Ev)
{
    if (Ev.IsRepeat()) {
        return false;
    }
    bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
    bool shift   = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

    // first priority
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
            if (control) {
                if (shift) {
                    SaveAs();
                }
                else {
                    SaveScene();
                }
            }
            break;
        }
        case Key::W:
        {
            if (control) {
                m_ActiveScene->DuplicateEntity(m_SceneHierarchyPanel.GetSelectedEntity());
            }
        }
        default:
            break;
    }

    auto selected_entity = m_SceneHierarchyPanel.GetSelectedEntity();
    if (ImGuizmo::IsUsing() && selected_entity) {
        switch (Ev.GetKeyCode()) {
            // Gizmos
            case Key::Q:
            {
                m_GizmoType = -1;
                break;
            }
            case Key::W:
            {
                {
                    m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
                }
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
        default:
            break;
    }
    return false;
}

void EditorLayer::NewScene()
{
    auto new_scene = CreateRef<Scene>(); // Just create a new scene/new tab(TODO)
    new_scene->OnViewportResize(m_ViewportSize.x, m_ViewportSize.y);
    SetActiveScene(new_scene);
}

void EditorLayer::OpenScene()
{


    auto path = FileDialogs::OpenFile("Hazel Scene (*.hazel)\0*.hazel\0"
                                      "Hazel Scene (*.yaml)\0*.yaml\0");
    if (!path.empty()) {
        OpenSceneImpl(path);
    }
}

void EditorLayer::OpenScene(const std::filesystem::path &path)
{
    HZ_CORE_ASSERT(!path.empty());

    OpenSceneImpl(path.string());
}

void EditorLayer::OpenSceneImpl(const std::string &path)
{
    // TODO: why? what if I want to add a runtime actor?
    if (m_SceneState == ESceneState::Play) {
        OnSceneStop();
    }


    if (!path.ends_with(".hazel")) {
        HZ_WARN("Could not load {0}, unsupported file type", path);
        return;
    }
    Ref<Scene>      new_scene = CreateRef<Scene>(); // Just create a new scene/new tab(TODO)
    SceneSerializer Serialize(new_scene);

    if (Serialize.Deserialize(path))
    {
        m_EditorScene = new_scene;
        m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_SceneHierarchyPanel.SetContext(m_EditorScene);

        SetActiveScene(m_EditorScene);
        m_ScenePath = path;
    }
}


void EditorLayer::SaveAs()
{
    auto path = FileDialogs::SaveFile("Hazel Scene (*.hazel)\0*.hazel\0"
                                      "Hazel Scene (*.yaml)\0*.yaml\0");

    bool Ok = SaveSceneImpl(m_ActiveScene, path);
    if (Ok) {
        m_ScenePath = path;
    }
}


void EditorLayer::SaveScene()
{
    if (!m_ScenePath) {
        SaveAs();
        return;
    }

    SaveSceneImpl(m_ActiveScene, m_ScenePath->string());
}

bool EditorLayer::SaveSceneImpl(const Ref<Scene> scene, std::string path)
{
    if (!path.empty()) {
        if (!FPath(path.c_str()).absolute_path.has_extension()) {
            path.append(".hazel");
        }
        SceneSerializer Serialize(scene);
        Serialize.Serialize(path);

        return true;
    }
    return false;
}

void EditorLayer::OnScenePlay()
{
    switch (m_SceneState) {

        case ESceneState::Stop:
        case ESceneState::Play:
            break;
        case ESceneState::Simulate:
        {
            OnSceneStop();
            break;
        }
    }
    m_SceneState   = ESceneState::Play;
    auto new_scene = Scene::Copy(m_EditorScene);
    SetActiveScene(new_scene);
    m_ActiveScene->OnRuntimeStart();
}

void EditorLayer::OnSceneSimulate()
{
    if (m_SceneState == ESceneState::Play) {
        OnSceneStop();
    }

    m_SceneState = ESceneState::Simulate;

    m_ActiveScene = Scene::Copy(m_EditorScene);
    m_ActiveScene->OnSimulationStart();

    m_SceneHierarchyPanel.SetContext(m_ActiveScene);
}

void EditorLayer::OnSceneStop()
{
    HZ_CORE_ASSERT(m_SceneState == ESceneState::Play || m_SceneState == ESceneState::Simulate);

    // TODO: pre-stop and post-stop
    switch (m_SceneState) {
        case ESceneState::Stop:
            break;
        case ESceneState::Play:
        {
            m_ActiveScene->OnRuntimeStop();
            break;
        }
        case ESceneState::Simulate:
        {
            m_ActiveScene->OnSimulationStop();
            break;
        }
    }
    m_SceneState = ESceneState::Stop;

    SetActiveScene(m_EditorScene);
}



} // namespace hazel
