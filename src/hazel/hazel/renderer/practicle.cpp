#include "glm/ext/vector_float3.hpp"
#include "hazel/core/log.h"
#include "hz_pch.h"



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


std::mt19937                                             Random::s_RandomEngine{};
std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution;

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

        practile.LifeRemaining -= ts;
        practile.Position += practile.Velocity * (float)ts;
        practile.Rotation += 0.01f * ts;
    }
}

void PraticleSystem::OnRender(hazel::OrthographicsCamera &camera)
{
    hazel::Render2D::BeginScene(camera);
    for (auto &practile : m_PraticlePool) {
        if (!practile.bActive) {
            continue;
        }
        float     life  = practile.LifeRemaining / practile.LifeTime;
        glm::vec4 color = glm::mix(practile.ColorEnd, practile.ColorBegin, life);

        float size = std::lerp(practile.SizeBegin, practile.SizeEnd, life);
        auto  pos  = glm::vec3{practile.Position, 5.f};
        hazel::Render2D::DrawRotateQuad(pos, {size, size}, glm::radians(practile.Rotation), color);
    }
    hazel::Render2D::EndScene();
    printf("%f %f\n",
           m_PraticlePool[m_PoolIndex].Position.x,
           m_PraticlePool[m_PoolIndex].Position.y);
}

void PraticleSystem::Emit(const PraticleProps &props)
{

    Praticle &praticle = m_PraticlePool[m_PoolIndex];

    praticle.bActive = true;

    praticle.Position = props.Position;
    praticle.Rotation = Random::Float() * 2.f * glm::pi<float>();

    praticle.Velocity = props.Velocity;
    praticle.Velocity.x += props.VelocityVariation.x * (Random::Float() - 0.5f);
    praticle.Velocity.y += props.VelocityVariation.x * (Random::Float() - 0.5f);

    praticle.ColorBegin = props.ColorBegin;
    praticle.ColorEnd   = props.ColorEnd;

    praticle.LifeTime      = props.LifeTime;
    praticle.LifeRemaining = props.LifeTime;

    praticle.SizeBegin = props.SizeBegin + props.SizeVariation * (Random::Float() - 0.5f);
    praticle.SizeBegin = props.SizeEnd;

    m_PoolIndex = (m_PoolIndex + 1) % m_PraticlePool.size();



    printf("%f %f\n", praticle.Position.x, praticle.Position.y);
}
