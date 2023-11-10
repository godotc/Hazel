//
// Created by nono on 2023-11-5
//

#include "hazel.h"
#include "hazel/renderer/practicle.h"


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


namespace hazel {

class EditorLayer : public hazel::Layer
{

    OrthographicsCameraController m_CameraController{16 / 9.f, true};

    glm::vec4 m_ClearColor{0.0, 0.0, 0.0, 0.0};
    glm::vec3 m_QuadPosition = glm::vec3(1, 1, 0);
    glm::vec4 m_FlatColor{1.f, 0.0, 0.0, 1.f};
    float     m_Shiftting[2] = {1, 1};

    Ref<Texture2D>    m_FaceTexture, m_ArchTexture, m_BlockTexture;
    Ref<Texture2D>    m_TinyTownSheet;
    Ref<SubTexture2D> m_SubBlock[12][11];
    Ref<SubTexture2D> m_WaterBuck;
    Ref<SubTexture2D> m_Tree;


    PraticleSystem m_PracticleSystem;
    PraticleProps  m_PracticleProps;

    Ref<Framebuffer> m_Framebuffer;

  public:
    EditorLayer();

    void OnAttach() override;
    void OnDetach() override;

    void OnUpdate(Timestep ts) override;

    void OnImGuiRender() override;

    void OnEvent(hazel::Event &event) override;

  private:
    void Init();
};


} // namespace hazel