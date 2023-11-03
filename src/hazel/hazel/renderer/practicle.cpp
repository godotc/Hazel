#include "hz_pch.h"

#include "glm/ext/vector_float3.hpp"
#include "hazel/core/log.h"



#include "glm/common.hpp"
#include "glm/ext/scalar_constants.hpp"
#include <cstdio>
#include <glm/gtx/common.hpp>
#include <glm/vec4.hpp>

#include "practicle.h"

#include "hazel/renderer/render_2d.h"
#include <cmath>
#include <fmt/format.h>
#include <glm/common.hpp>



#include <cstdint>
#include <limits>
#include <random>
class Random
{
    // it need a seed, but will same when seed are the saem
    static std::mt19937 s_RandomNumberGenerator;
    // so use this waste performance to get the random seed
    static std::random_device s_RandomDevice;

    static std::uniform_int_distribution<int>    s_UnifromDistribution_Int;
    static std::uniform_real_distribution<float> s_UnifromDistribution_Float;

  public:
    /**
     * @brief  Get [0.0, 1.0)
     * @usage: Random::Float() - 0.5f
     * @return float
     */
    static float Float() { return s_UnifromDistribution_Float(s_RandomNumberGenerator); }
};

std::mt19937                          Random::s_RandomNumberGenerator{Random::s_RandomDevice()};
std::random_device                    Random::s_RandomDevice{};
std::uniform_real_distribution<float> Random::s_UnifromDistribution_Float{0.f, 1.f};


PraticleSystem::PraticleSystem()
{
    m_PraticlePool.resize(1000);
}


void PraticleSystem::OnUpdate(hazel::Timestep ts)
{
    for (auto &practile : m_PraticlePool) {
        if (!practile.bActive) {
            continue;
        }

        if (practile.LifeRemaining <= 0.f) {
            practile.bActive = false;
            continue;
        }

        practile.LifeRemaining -= ts.GetSeconds();

        // printf("old  %f %f\n", practile.Position.x, practile.Position.y);
        practile.Position += practile.Velocity * ts.GetSeconds();
        // printf("new  %f %f\n", practile.Position.x, practile.Position.y);


        practile.Rotation += 0.01f * ts;
    }
}

void PraticleSystem::OnRender(hazel::OrthographicsCamera &camera)
{
    hazel::Render2D::BeginScene(camera);

    for (auto &practile : m_PraticlePool)
    {
        if (!practile.bActive) {
            continue;
        }

        float life = practile.LifeRemaining / practile.LifeTime;

        glm::vec4 color = glm::mix(practile.ColorEnd, practile.ColorBegin, life);
        float     size  = std::lerp(practile.SizeBegin, practile.SizeEnd, life);
        glm::vec3 pos   = glm::vec3{practile.Position, 0.2f};


        // printf("drew on %f %f\n", pos.x, pos.y);
        hazel::Render2D::DrawRotateQuad(pos, {size, size}, glm::radians(practile.Rotation), color);
    }
    hazel::Render2D::EndScene();
}

void PraticleSystem::Emit(const PraticleProps &props)
{
    Praticle &praticle = m_PraticlePool[m_PoolIndex];

    praticle.bActive = true;

    praticle.Position = props.Position;
    praticle.Rotation = Random::Float() * 2.f * glm::pi<float>();

    praticle.Velocity = props.Velocity;


    // printf("the random flaot %f\n", Random::Float());
    // printf("old  %f %f\n", praticle.Velocity.x, praticle.Velocity.y);
    praticle.Velocity.x += props.VelocityVariation.x * (Random::Float() - 0.5f);
    praticle.Velocity.y += props.VelocityVariation.y * (Random::Float() - 0.5f);
    // printf("new  %f %f\n", praticle.Velocity.x, praticle.Velocity.y);

    praticle.ColorBegin = props.ColorBegin;
    praticle.ColorEnd   = props.ColorEnd;

    praticle.LifeTime      = props.LifeTime;
    praticle.LifeRemaining = props.LifeTime;

    praticle.SizeBegin = props.SizeBegin + props.SizeVariation * (Random::Float() - 0.5f);
    praticle.SizeEnd   = props.SizeEnd;

    m_PoolIndex = (m_PoolIndex + 1) % m_PraticlePool.size();


    // printf("emit on %f %f\n", praticle.Position.x, praticle.Position.y);
}
