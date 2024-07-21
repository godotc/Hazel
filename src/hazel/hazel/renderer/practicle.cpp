#include "hz_pch.h"

#include "glm/ext/vector_float3.hpp"



#include "glm/common.hpp"
#include "glm/ext/scalar_constants.hpp"
#include <glm/vec4.hpp>

#include "practicle.h"

#include "hazel/renderer/render_2d.h"
#include <fmt/format.h>
#include <glm/common.hpp>



#include <cstdint>
#include <random>
class Random
{
    // it need a seed, but will same when seed are the same
    static std::mt19937 s_RandomNumberGenerator;

    // so use this waste performance to get the random seed
    // static std::random_device s_RandomDevice;

    static std::uniform_int_distribution<int>    s_UniformDistribution_Int;
    static std::uniform_real_distribution<float> s_UniformDistribution_Float;

  public:
    /**
     * @brief  Get [0.0, 1.0)
     * @usage: Random::Float() - 0.5f
     * @return float
     */
    static float Float() { return s_UniformDistribution_Float(s_RandomNumberGenerator); }
};

// std::random_device                    Random::editor counts_RandomDevice{};
std::mt19937                          Random::s_RandomNumberGenerator{std::random_device()()};
std::uniform_real_distribution<float> Random::s_UniformDistribution_Float{0.f, 1.f};


ParticleSystem::ParticleSystem(uint32_t max_particles)
{
    m_ParticlePool.resize(max_particles);
}


void ParticleSystem::OnUpdate(hazel::Timestep ts)
{
    for (auto &particle : m_ParticlePool) {
        if (!particle.bActive) {
            continue;
        }

        if (particle.LifeRemaining <= 0.f) {
            particle.bActive = false;
            continue;
        }

        particle.LifeRemaining -= ts.GetSeconds();

        // printf("old  %f %f\n", particle.Position.x, particle.Position.y);
        particle.Position += particle.Velocity * ts.GetSeconds();
        // printf("new  %f %f\n", particle.Position.x, particle.Position.y);


        particle.Rotation += 0.01f * ts;
    }
}

void ParticleSystem::OnRender(hazel::OrthographicCamera &camera)
{
    hazel::Render2D::BeginScene(camera);

    for (auto &particle : m_ParticlePool)
    {
        if (!particle.bActive) {
            continue;
        }

        float life = particle.LifeRemaining / particle.LifeTime;

        glm::vec4 color = glm::mix(particle.ColorEnd, particle.ColorBegin, life);
        float     size  = std::lerp(particle.SizeBegin, particle.SizeEnd, life);
        glm::vec3 pos   = glm::vec3{particle.Position, 0.2f};


        // printf("drew on %f %f\n", pos.x, pos.y);
        hazel::Render2D::DrawRotateQuad(pos, {size, size}, glm::radians(particle.Rotation), color);
    }
    hazel::Render2D::EndScene();
}

void ParticleSystem::Emit(const ParticleProps &props)
{
    Particle &particle = m_ParticlePool[m_PoolIndex];

    particle.bActive = true;

    particle.Position = props.Position;
    particle.Rotation = Random::Float() * 2.f * glm::pi<float>();

    particle.Velocity = props.Velocity;


    // printf("the random float %f\n", Random::Float());
    // printf("old  %f %f\n", particle.Velocity.x, particle.Velocity.y);
    particle.Velocity.x += props.VelocityVariation.x * (Random::Float() - 0.5f);
    particle.Velocity.y += props.VelocityVariation.y * (Random::Float() - 0.5f);
    // printf("new  %f %f\n", particle.Velocity.x, particle.Velocity.y);

    particle.ColorBegin = props.ColorBegin;
    particle.ColorEnd   = props.ColorEnd;

    particle.LifeTime      = props.LifeTime;
    particle.LifeRemaining = props.LifeTime;

    particle.SizeBegin = props.SizeBegin + props.SizeVariation * (Random::Float() - 0.5f);
    particle.SizeEnd   = props.SizeEnd;

    m_PoolIndex = (m_PoolIndex + 1) % m_ParticlePool.size();


    // printf("emit on %f %f\n", particle.Position.x, particle.Position.y);
}
