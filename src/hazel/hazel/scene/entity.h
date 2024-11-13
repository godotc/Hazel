/**
 * @ Author: godot42
 * @ Create Time: 2024-08-15 22:17:08
 * @ Modified by: @godot42
 * @ Modified time: 2024-11-13 17:50:25
 * @ Description:
 */

#pragma once



#include "component.h"

#include "entt/entity/registry.hpp"
#include "hazel/core/base.h"
#include "hazel/core/uuid.h"
#include "scene.h"
#include <cstdint>
#include <entt/entt.hpp>
#include <string>
#include <type_traits>

namespace hazel {


class HAZEL_API Entity
{
    entt::entity m_EntityHandle = entt::null;
    Scene       *m_Scene        = nullptr;


  public:
    Entity() = default;
    Entity(entt::entity handle, Scene *scene);
    Entity(const Entity &other) = default;
    UUID        GetUUID() const;
    std::string GetName() const { return m_Scene->m_Registry.get<TagComponent>(m_EntityHandle).Tag; }

    operator bool() const { return m_EntityHandle != entt::null && m_Scene->m_Registry.valid(m_EntityHandle); }
    operator uint32_t() const { return uint32_t(m_EntityHandle); }
    operator entt::entity() const { return m_EntityHandle; }

    bool operator==(Entity &Other) { return m_EntityHandle == Other.m_EntityHandle; }

    template <class ComponentType>
    bool HasComponent() const
    {
        return m_Scene->m_Registry.all_of<ComponentType>(m_EntityHandle);
    }

    template <class ComponentType, class... Args>
    ComponentType &AddComponent(Args &&...args)
        requires(std::is_base_of_v<Component<ComponentType>, ComponentType>)
    {
        HZ_CORE_ASSERT(!HasComponent<ComponentType>(), "Entity already has that component!");
        auto &comp = m_Scene->m_Registry.emplace<ComponentType>(m_EntityHandle, std::forward<Args>(args)...);
        comp.OnComponentAdded(m_Scene);
        return comp;
    }

    template <class ComponentType, class... Args>
    ComponentType &AddOrReplaceComponent(Args &&...args)
    {
        auto &comp = m_Scene->m_Registry.emplace_or_replace<ComponentType>(m_EntityHandle, std::forward<Args>(args)...);
        comp.OnComponentAdded(m_Scene);
        return comp;
    }

    template <class ComponentType>
    ComponentType &GetComponent()
    {
        HZ_CORE_ASSERT(HasComponent<ComponentType>(), "Entity did not has that component!");
        return m_Scene->m_Registry.get<ComponentType>(m_EntityHandle);
    }

    template <class ComponentType>
    const ComponentType &GetComponent() const
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