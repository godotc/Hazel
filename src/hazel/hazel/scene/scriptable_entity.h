#pragma once

#include "entity.h"
#include "hazel/core/timestep.h"

namespace hazel {

class ScriptableEntity
{
    friend class Scene;

    Entity m_Entity;

  public:
    virtual ~ScriptableEntity() = default;


    template <class ComponentType>
    ComponentType &GetComponent()
    {
        return m_Entity.GetComponent<ComponentType>();
    }

    virtual void OnCreate() {}
    virtual void OnUpdate(Timestep ts) {}
    virtual void OnDestroy(Timestep ts) {}
};

} // namespace hazel