#pragma once
#include "__microshit_api_hazel.h"

#include "entt/entity/fwd.hpp"
#include "hazel/core/timestep.h"
#include "hazel/scene/editor_camera.h"
#include <entt/entt.hpp>



struct b2World;
namespace hazel {

class Entity;
class TransformComponent;
class CameraComponent;
class TagComponent;
class SpriteRendererComponent;
class NativeScriptComponent;


using Entity_t = entt::entity;

class HAZEL_API Scene
{
    friend class Entity;
    friend class SceneHierarchyPanel;
    friend class SceneSerializer;

    entt::registry m_Registry;
    uint32_t       m_ViewportWidth = 0, m_ViewportHeight = 0;
    b2World       *m_PhysicsWorld = nullptr;


  public:
    Scene();
    virtual ~Scene() = default;

    void OnRuntimeStart();
    void OnRuntimeStop();

    void OnUpdateEditor(Timestep ts, EditorCamera &camera);
    void OnUpdateRuntime(Timestep ts);
    void OnViewportResize(uint32_t w, uint32_t h);

  public:
    Entity GetPrimaryCameraEntity();

    Entity CreateEntity(const std::string &name);
    void   DestroyEntity(Entity entity);

  private:
    template <class T>
    void OnComponentAdded(Entity entity, T &component);
    template <>
    void OnComponentAdded<TransformComponent>(Entity entity, TransformComponent &component);
    template <>
    void OnComponentAdded<CameraComponent>(Entity entity, CameraComponent &component);
    template <>
    void OnComponentAdded<TagComponent>(Entity entity, TagComponent &component);
    template <>
    void OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent &component);
    template <>
    void OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent &component);
};



} // namespace hazel