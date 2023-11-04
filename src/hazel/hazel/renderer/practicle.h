#pragma once


#include "__microshit_api_hazel.h"
#include "hazel/core/timestep.h"
#include "hazel/renderer/orthographic_camera.h"

#include "glm/fwd.hpp"
#include <cstdint>



struct PraticleProps {
    glm::vec2 Position;
    glm::vec2 Velocity, VelocityVariation;
    glm::vec4 ColorBegin, ColorEnd;
    float     SizeBegin, SizeEnd, SizeVariation;
    float     LifeTime = 1.f;
};

class HAZEL_API PraticleSystem
{
    struct Praticle {
        glm::vec2 Position;
        glm::vec2 Velocity;
        glm::vec4 ColorBegin, ColorEnd;
        float     Rotation = 0.f;
        float     SizeBegin, SizeEnd;

        float LifeTime      = 1.f;
        float LifeRemaining = 0.f;

        bool bActive = false;
    };

    std::vector<Praticle> m_PraticlePool;
    uint32_t              m_PoolMax   = 999;
    int32_t               m_PoolIndex = 0;


  public:
    explicit PraticleSystem(uint32_t max_praticles = 10000);

    void OnUpdate(hazel::Timestep ts);
    void OnRender(hazel::OrthographicsCamera &camera);

    void Emit(const PraticleProps &props);
};
