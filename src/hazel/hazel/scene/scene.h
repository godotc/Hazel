#pragma once
#include "entt/entity/fwd.hpp"
#include "hazel/core/timestep.h"
#include <entt/fwd.hpp>

#include <entt/entt.hpp>

#include "component.h"

namespace hazel {

class Scene
{
    entt::registry m_Registry;

  public:
    Scene();
    virtual ~Scene() = default;

    void OnUpdate(Timestep ts);

  public:
    entt::registry &GetRegistry() { return m_Registry; }
    entt::entity    CreateEntity();
};


} // namespace hazel