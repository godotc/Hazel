#pragma once
#include "__microshit_api_hazel.h"
#include "entt/entity/fwd.hpp"
#include "hazel/core/timestep.h"
#include <entt/fwd.hpp>

#include <entt/entt.hpp>

#include "component.h"

namespace hazel {

class Entity;

class HAZEL_API Scene
{
    friend class Entity;

    entt::registry m_Registry;

  public:
    Scene();
    virtual ~Scene() = default;

    void OnUpdate(Timestep ts);

  public:
    Entity CreateEntity(const std::string &name);
};


} // namespace hazel