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