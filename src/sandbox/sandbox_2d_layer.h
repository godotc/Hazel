//
// Created by nono on 10/11/23.
//


#include <hazel.h>

#include "hazel/renderer/practicle.h"
#include "hazel/renderer/subtexture_2d.h"


class Sandbox2D : public hazel::Layer
{
  public:
    Sandbox2D() : hazel::Layer("Sandbox2D") {}
    ~Sandbox2D() override = default;
    void OnAttach() override;
    void OnDetach() override;

    void OnUpdate(hazel::Timestep timestep) override;
    void OnEvent(hazel::Event &event) override;

    void OnImGuiRender() override;

  private:
    hazel::ShaderLibrary m_ShaderLibrary;

    hazel::OrthographicsCameraController m_CameraController{16 / 9.f, true};

    glm::vec4 m_ClearColor{0.0, 0.0, 0.0, 0.0};
    glm::vec3 m_QuadPosition = glm::vec3(1, 1, 0);
    glm::vec4 m_FlatColor{1.f, 0.0, 0.0, 1.f};
    float     m_Shiftting[2] = {1, 1};

    hazel::Ref<hazel::Texture2D>    m_FaceTexture, m_ArchTexture, m_BlockTexture;
    hazel::Ref<hazel::Texture2D>    m_TinyTownSheet;
    hazel::Ref<hazel::SubTexture2D> m_SubBlock[12][11];
    hazel::Ref<hazel::SubTexture2D> m_WaterBuck;
    hazel::Ref<hazel::SubTexture2D> m_Tree;

    PraticleSystem m_PracticleSystem;
    PraticleProps  m_PracticleProps;
};
