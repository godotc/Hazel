//
// Created by nono on 10/11/23.
//


#include "hazel/hazel.h"



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
    hazel::OrthographicsCameraController m_CameraController{16 / 9.f, true};
    glm::vec4                            m_ClearColor{0.3, 0.4, 0.5, 1};
    hazel::ShaderLibrary                 m_ShaderLibrary;

    //
    hazel::Ref<hazel::VertexArray> m_FlatVA;

    glm::vec3                 m_QuadPosition = glm::vec3(0.f);
    hazel::Ref<hazel::Shader> m_FlatShader;
    glm::vec4                 m_FlatColor{0, 0.7, 0.3, 0.5};

    hazel::Ref<hazel::Texture2D> m_Texture, m_ArchTexture;
};
