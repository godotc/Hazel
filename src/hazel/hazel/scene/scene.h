#pragma once
#include "__microshit_api_hazel.h"
#include "entt/entity/fwd.hpp"
#include "hazel/core/timestep.h"
#include <cstdint>
#include <entt/fwd.hpp>

#include <entt/entt.hpp>


namespace hazel {

class Entity;

class HAZEL_API Scene
{
    friend class Entity;
    friend class SceneHierarchyPanel;

    entt::registry m_Registry;
    uint32_t       m_ViewportWidth = 0, m_ViewportHeight = 0;


  public:
    Scene();
    virtual ~Scene() = default;

    void OnUpdate(Timestep ts);
    void OnViewportResize(uint32_t w, uint32_t h);

  public:
    Entity CreateEntity(const std::string &name);
};


} // namespace hazel