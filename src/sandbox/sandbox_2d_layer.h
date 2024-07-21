//
// Created by nono on 10/11/23.
//

#include "hazel.h"

#include "entt/entity/fwd.hpp"


#include "hazel/core/base.h"
#include "hazel/renderer/practicle.h"
#include "hazel/scene/scene.h"


static std::unordered_map<char, std::array<int, 2>> tile_block_map = {
    {'w', {0, 10}},
    {'R',  {1, 8}},
    {'W', {1, 10}},
    {'G', {5, 10}},
    {'F',  {9, 7}},
    {'f',  {8, 6}},
    {'B', {11, 0}},
};

const int         map_width  = 24;
const int         map_height = 12;
const std::string game_map   = "fFFFFFFFFFFFFFFFFFFFFFFf"
                               "fWWWWWWWWWWWWWWWWWWWWWWf"
                               "fRWGWWWWWWWWWWWWGWWWWWWf"
                               "fRWWWWWBWWWBWWWWWWWWWWWf"
                               "fRWWGWWWWWWWWWWWGWWWWWWf"
                               "fRWWWWWWWWWWWWWWWWWWWWWf"
                               "fRWWGWWWWWWBWWWWWWWWWWWf"
                               "fRWWWWWWBWWWWWWWGWWWWWWf"
                               "fRWWGWWWWWWWWWGGGWWWWWWf"
                               "fRWWWWWWWWWWWWWWWWWWWWWf"
                               "fWWRRRRRRRRRRRRRRRRWWWWf"
                               "fFFFFFFFFFFFFFFFFFFFFFFf";


class Sandbox2D : public hazel::Layer
{

    hazel::ShaderLibrary m_ShaderLibrary;

    hazel::OrthographicCameraController m_CameraController{16 / 9.f, true};

    glm::vec4 m_ClearColor{0.0, 0.0, 0.0, 0.0};
    glm::vec3 m_QuadPosition = glm::vec3(1, 1, 0);
    glm::vec4 m_FlatColor{1.f, 0.0, 0.0, 1.f};
    float     m_Shiftting[2] = {1, 1};

    hazel::Ref<hazel::Texture2D>    m_FaceTexture, m_ArchTexture, m_BlockTexture;
    hazel::Ref<hazel::Texture2D>    m_TinyTownSheet;
    hazel::Ref<hazel::SubTexture2D> m_SubBlock[12][11];
    hazel::Ref<hazel::SubTexture2D> m_WaterBuck;
    hazel::Ref<hazel::SubTexture2D> m_Tree;


    ParticleSystem m_PracticleSystem;
    ParticleProps  m_PracticleProps;

    hazel::Ref<hazel::Framebuffer> m_Framebuffer;

    hazel::Ref<hazel::Scene> m_ActiveScene;
    hazel::Entity            m_SquareEntity;


  public:
    Sandbox2D() : hazel::Layer("Sandbox2D") {}
    ~Sandbox2D() override = default;
    void OnAttach() override;
    void OnDetach() override;

    void OnUpdate(hazel::Timestep timestep) override;
    void OnEvent(hazel::Event &event) override;

    void OnImGuiRender() override;

  private:
    void Init();
};
