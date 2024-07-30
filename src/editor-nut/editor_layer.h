//
// Created by nono on 2023-11-5
//

#include "hazel.h"

#include "glm/ext/vector_float2.hpp"
#include "hazel/event/key_event.h"
#include "hazel/scene/editor_camera.h"
#include "hazel/scene/scene.h"
#include "panel/scene_hierachy_panel.h"
#include "panel/content_browser_panel.h"



namespace hazel {

class EditorLayer : public hazel::Layer
{
    Ref<Scene> m_ActiveScene;

    // Panel
    SceneHierarchyPanel m_SceneHierarchyPanel;
    ContentBrowserPanel m_ContentBrowserPanel;
    EditorCamera        m_EditorCamera;

    OrthographicCameraController m_CameraController{16 / 9.f, true};

    // ParticleSystem m_PracticeSystem;
    // ParticleProps  m_PracticeProps;
    int m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;

    bool               bFullscreen      = true;
    bool               bPadding         = true;
    ImGuiDockNodeFlags m_DockspaceFlags = ImGuiDockNodeFlags_None;
    ImGuiWindowFlags   m_WindowFlags    = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    // ----------------------

    glm::vec2 m_ViewportSize = {0.f, 0.f};
    glm::vec2 m_ViewportBounds[2];
    glm::vec4 m_ClearColor{0.0, 0.0, 0.0, 1.0};

    Ref<Framebuffer> m_Framebuffer;
    bool             bViewPortFocusing = false;
    bool             bViewPortHovering = false;

    Entity m_HoveredEntity;

    int m_ViewportColorAttachmentId = 0;

  public:
    EditorLayer();
    ~EditorLayer();

    void OnAttach() override;
    void OnDetach() override;

    void OnUpdate(Timestep ts) override;
    void OnImGuiRender() override;

    void OnEvent(hazel::Event &event) override;

  private:

    // --- GUI renderer
    void UpdateWindowFlags();
    void MenuBar();
    void ViewPort();
    void FontSwitcher();

    void Settings();
    void RenderStats();

    // Must call in viewport draw process(in the same window)
    void Gizmos();

    // --- Events
    bool OnKeyPressed(const KeyPressedEvent &Ev);
    bool OnMouseButtonPressed(const MouseButtonPressedEvent &Ev);

    // ---
    void NewScene();
    void OpenScene();
    void SaveAs();
};


} // namespace hazel