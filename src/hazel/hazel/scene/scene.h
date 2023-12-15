#pragma once
#include "__microshit_api_hazel.h"

#include "entt/entity/fwd.hpp"
#include "hazel/core/timestep.h"
#include <entt/entt.hpp>



namespace hazel {

class Entity;
class TransformComponent;
class CameraComponent;
class TagComponent;
class SpriteRendererComponent;
class NativeScriptComponent;


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
    void   DestoryEntity(Entity entity);

  private:
    template <class T>
    void OnComponentAdd(Entity entity, T &component);
    template <>
    void OnComponentAdd<TransformComponent>(Entity entity, TransformComponent &component);
    template <>
    void OnComponentAdd<CameraComponent>(Entity entity, CameraComponent &component);
    template <>
    void OnComponentAdd<TagComponent>(Entity entity, TagComponent &component);
    template <>
    void OnComponentAdd<SpriteRendererComponent>(Entity entity, SpriteRendererComponent &component);
    template <>
    void OnComponentAdd<NativeScriptComponent>(Entity entity, NativeScriptComponent &component);
};



} // namespace hazel