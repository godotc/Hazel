#pragma once


#include "entity.h"

namespace hazel {

class ScriptableEntity
{
    friend class Scene;

    Entity m_Entity;

  public:


    template <class ComponentType>
    ComponentType &GetComponent()
    {
        return m_Entity.GetComponent<ComponentType>();
    }
};

} // namespace hazel