/**
 * @ Author: godot42
 * @ Create Time: 2023-11-23 00:28:22
 * @ Modified by: @godot42
 * @ Modified time: 2024-08-18 03:35:21
 * @ Description:
 */

#include "entity.h"

namespace hazel {

Entity::Entity(entt::entity handle, Scene *scene)
{
    m_EntityHandle = handle;
    m_Scene        = scene;
}

UUID Entity::Entity::GetUUID() const
{
    return m_Scene->m_Registry.get<IDComponent>(m_EntityHandle).ID;
}

} // namespace hazel