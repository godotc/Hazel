//
// Created by nono on 2023-11-5
//

#include "hazel.h"

#include "glm/ext/vector_float2.hpp"
#include "hazel/renderer/practicle.h"
#include "hazel/scene/scene.h"



namespace hazel {

class EditorLayer : public hazel::Layer
{

    Ref<Framebuffer> m_Framebuffer;
    bool             bViewPortFocusing = false;
    bool             bViewPortHovering = false;

    Ref<Scene> m_ActiveScene;
    Entity     m_CameraEntity;
    Entity     m_SecondCameraEntity;
    Entity     m_SquareEntity;

    bool bPrimaryCamera = true;


    OrthographicsCameraController m_CameraController{16 / 9.f, true};
    glm::vec2                     m_ViewportSize = {0.f, 0.f};

    glm::vec4 m_ClearColor{0.0, 0.0, 0.0, 1.0};
    glm::vec3 m_QuadPosition = glm::vec3(1, 1, 0);
    glm::vec4 m_FlatColor{1.f, 0.0, 0.0, 1.f};
    float     m_Shiftting[2] = {1, 1};


    Ref<Texture2D> m_FaceTexture, m_ArchTexture, m_BlockTexture;

    Ref<Texture2D>    m_TinyTownSheet;
    Ref<SubTexture2D> m_SubBlock[12][11];
    Ref<SubTexture2D> m_WaterBuck;
    Ref<SubTexture2D> m_Tree;


    PraticleSystem m_PracticleSystem;
    PraticleProps  m_PracticleProps;


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
};


} // namespace hazel