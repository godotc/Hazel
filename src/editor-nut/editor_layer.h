//
// Created by nono on 2023-11-5
//

#include "hazel.h"

#include "glm/ext/vector_float2.hpp"
#include "hazel/renderer/practicle.h"
#include "hazel/scene/scene.h"
#include "panel/scene_hierachy_panel.h"



namespace hazel {

class EditorLayer : public hazel::Layer
{
    glm::vec2 m_ViewportSize = {0.f, 0.f};
    glm::vec4 m_ClearColor{0.0, 0.0, 0.0, 1.0};

    Ref<Framebuffer> m_Framebuffer;
    bool             bViewPortFocusing = false;
    bool             bViewPortHovering = false;

    Ref<Scene> m_ActiveScene;
    // Entities
    Entity m_CameraEntity;
    Entity m_SecondCameraEntity;
    Entity m_SquareEntity;
    Entity m_GreenSquareEntity;

    // Pannel
    SceneHierarchyPanel m_SceneHierachyPanel;

    Ref<Texture2D> m_FaceTexture, m_ArchTexture, m_BlockTexture;

    OrthographicsCameraController m_CameraController{16 / 9.f, true};


    // PraticleSystem m_PracticleSystem;
    // PraticleProps  m_PracticleProps;


  public:
    EditorLayer();
    ~EditorLayer();

    void OnAttach() override;
    void OnDetach() override;

    void OnUpdate(Timestep ts) override;
    void OnImGuiRender() override;
    void OnEvent(hazel::Event &event) override;

  private:
    void Init();

    void ViewPort();
    void FontSwitcher();
};


} // namespace hazel