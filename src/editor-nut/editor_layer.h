//
// Created by nono on 2023-11-5
//

#include "hazel.h"

#include "glm/ext/vector_float2.hpp"
#include "hazel/event/key_event.h"
#include "hazel/renderer/practicle.h"
#include "hazel/scene/editor_camera.h"
#include "hazel/scene/scene.h"
#include "panel/scene_hierachy_panel.h"



namespace hazel {

class EditorLayer : public hazel::Layer
{
    Ref<Scene> m_ActiveScene;

    // Pannel
    SceneHierarchyPanel m_SceneHierachyPanel;
    EditorCamera        m_EditorCamera;

    OrthographicsCameraController m_CameraController{16 / 9.f, true};

    // PraticleSystem m_PracticleSystem;
    // PraticleProps  m_PracticleProps;
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

    // Entities
    Entity m_CameraEntity;
    Entity m_SecondCameraEntity;
    Entity m_SquareEntity;
    Entity m_GreenSquareEntity;

    Ref<Texture2D> m_FaceTexture, m_ArchTexture, m_BlockTexture;


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
    // Must call in viewprot draw process(in the same window)
    void Gizmos();

    // --- Events
    bool OnKeyPressed(const KeyPressedEvent &Ev);

    // ---
    void NewScene();
    void OpenScene();
    void SaveAs();
};


} // namespace hazel