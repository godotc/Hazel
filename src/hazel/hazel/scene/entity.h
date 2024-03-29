#pragma once

#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"

#include "entt/entity/handle.hpp"
#include "entt/entity/registry.hpp"
#include "hazel/core/base.h"
#include "scene.h"
#include <cstdint>
#include <entt/entt.hpp>

namespace hazel {


class HAZEL_API Entity
{
    entt::entity m_EntityHandle = entt::null;
    Scene       *m_Scene        = nullptr;


  public:
    Entity() = default;
    Entity(entt::entity handle, Scene *scene);
    Entity(const Entity &other) = default;

    operator bool() const { return m_EntityHandle != entt::null && m_Scene->m_Registry.valid(m_EntityHandle); }
    operator uint32_t() const { return uint32_t(m_EntityHandle); }
    operator entt::entity() const { return m_EntityHandle; }

    bool operator==(Entity &Other) { return m_EntityHandle == Other.m_EntityHandle; }

    template <class ComponentType>
    bool HasComponent()
    {
        return m_Scene->m_Registry.all_of<ComponentType>(m_EntityHandle);
    }

    template <class ComponentType, class... Args>
    ComponentType &AddComponent(Args &&...args)
    {
        HZ_CORE_ASSERT(!HasComponent<ComponentType>(), "Entity already has that component!");
        auto &comp = m_Scene->m_Registry.emplace<ComponentType>(m_EntityHandle, std::forward<Args>(args)...);
        m_Scene->OnComponentAdd<ComponentType>(*this, comp);
        return comp;
    }

    template <class ComponentType>
    ComponentType &GetComponent()
    {
        HZ_CORE_ASSERT(HasComponent<ComponentType>(), "Entity did not has that component!");
        return m_Scene->m_Registry.get<ComponentType>(m_EntityHandle);
    }

    template <class ComponentType>
    ComponentType &GetOrAddComponent()
    {
        return m_Scene->m_Registry.get_or_emplace<ComponentType>(m_EntityHandle);
    }

    template <class ComponentType>
    uint32_t RemoveComponent()
    {
        HZ_CORE_ASSERT(HasComponent<ComponentType>(), "Entity did not has that component!");
        return m_Scene->m_Registry.remove<ComponentType>(m_EntityHandle);
    }
};

} // namespace hazel