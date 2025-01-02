/**
 * @ Author: godot42
 * @ Create Time: 2024-08-18 00:50:42
 * @ Modified by: @godot42
 * @ Modified by: @godot42
 * @ Modified time: 2025-01-02 21:07:45
 */

//
// Created by nono on 2023-11-5
//

#include "hazel.h"

#include "glm/ext/vector_float2.hpp"
#include "hazel/core/base.h"
#include "hazel/event/key_event.h"
#include "hazel/renderer/texture.h"
#include "hazel/scene/component.h"
#include "hazel/scene/editor_camera.h"
#include "hazel/scene/entity.h"
#include "hazel/scene/scene.h"
#include "panel/content_browser_panel.h"
#include "panel/scene_hierachy_panel.h"
#include <optional>



namespace hazel {


class EditorLayer : public hazel::Layer
{
  public:
    enum class ESceneState
    {
        Stop = 0,
        Play,
        Simulate,
    };

  private:
    // Scene
    Ref<Scene>                           m_ActiveScene;
    Ref<Scene>                           m_EditorScene;
    ESceneState                          m_SceneState = ESceneState::Stop;
    std::optional<std::filesystem::path> m_ScenePath;

    // Panel
    SceneHierarchyPanel m_SceneHierarchyPanel;
    ContentBrowserPanel m_ContentBrowserPanel;

    EditorCamera m_EditorCamera;

    bool bDefaultMaximizeWindow = true;

    OrthographicCameraController m_CameraController{16 / 9.f, true};

    // ParticleSystem m_PracticeSystem;
    // ParticleProps  m_PracticeProps;
    int m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;

    bool               bFullscreen      = true;
    bool               bPadding         = true;
    ImGuiDockNodeFlags m_DockspaceFlags = ImGuiDockNodeFlags_None;
    ImGuiWindowFlags   m_WindowFlags    = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    // window show state flags
    bool bShowRenderStats    = true;
    bool bShowSettingsWindow = true;

    bool      bShow2DPhysicsCollisions = true;
    glm::vec4 m_DebugCollisionColor    = {0.f, 1.f, 0.f, 1.f};

    // ----------------------

    glm::vec2 m_ViewportSize = {0.f, 0.f};
    glm::vec2 m_ViewportBounds[2];
    glm::vec4 m_ClearColor{0.0, 0.0, 0.0, 1.0};

    Ref<Framebuffer> m_Framebuffer;
    bool             bViewPortFocusing = false;
    bool             bViewPortHovering = false;

    Entity m_HoveredEntity;

    int m_ViewportColorAttachmentId = 0;

    float m_DebugFloat = 1.f;

    // Ed resources
    Ref<Texture2D> m_IconPlay;
    Ref<Texture2D> m_IconStop;
    Ref<Texture2D> m_IconSimulate;

  public:
    EditorLayer();
    ~EditorLayer();

    void OnAttach() override;
    void OnDetach() override;

    void OnUpdate(Timestep ts) override;
    void OnImGuiRender() override;

    void OnEvent(hazel::Event &event) override;


    // update it when have project
    static std::filesystem::path DefaultAssetsDirectory();
    static void                  SetDefaultAssetsDirectory(const std::filesystem::path &path);

    void SetActiveScene(Ref<Scene> new_scene);

  private:
    // Overlay
    void OnOverlayRender();

    //
    void OnDebugRender();


    // --- GUI renderer
    void UpdateWindowFlags();
    void MenuBar();
    void ViewPort();
    void UI_Toolbar();

    void UI_FontSwitcher();
    void UI_Settings();
    void Win_RenderStats();

    // Must call in viewport draw process(in the same window)
    void RenderGizmos();

    // --- Events
    bool OnKeyPressed(const KeyPressedEvent &Ev);
    bool OnMouseButtonPressed(const MouseButtonPressedEvent &Ev);

    // --- Scene Create/Load/Save
    void NewScene();

    void OpenScene();
    void OpenScene(const std::filesystem::path &path);
    void OpenSceneImpl(const std::string &path);

    void SaveAs();
    void SaveScene();
    bool SaveSceneImpl(const Ref<Scene> scene, std::string path);
    // ---


    void OnScenePlay();
    void OnSceneBeginPlay();

    void OnSceneSimulate();
    void OnSceneStop();
};


} // namespace hazel
