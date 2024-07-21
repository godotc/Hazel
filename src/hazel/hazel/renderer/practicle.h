#pragma once


#include "__microshit_api_hazel.h"
#include "hazel/core/timestep.h"
#include "hazel/renderer/orthographic_camera.h"

#include "glm/fwd.hpp"
#include <cstdint>



struct ParticleProps {
    glm::vec2 Position;
    glm::vec2 Velocity, VelocityVariation;
    glm::vec4 ColorBegin, ColorEnd;
    float     SizeBegin, SizeEnd, SizeVariation;
    float     LifeTime = 1.f;
};

class HAZEL_API ParticleSystem
{
    struct Particle {
        glm::vec2 Position;
        glm::vec2 Velocity;
        glm::vec4 ColorBegin, ColorEnd;
        float     Rotation = 0.f;
        float     SizeBegin, SizeEnd;

        float LifeTime      = 1.f;
        float LifeRemaining = 0.f;

        bool bActive = false;
    };

    std::vector<Particle> m_ParticlePool;
    uint32_t              m_PoolMax   = 999;
    int32_t               m_PoolIndex = 0;


  public:
    explicit ParticleSystem(uint32_t max_particles = 10000);

    void OnUpdate(hazel::Timestep ts);
    void OnRender(hazel::OrthographicCamera &camera);

    void Emit(const ParticleProps &props);
};
