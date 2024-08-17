/**
 * @ Author: godot42
 * @ Create Time: 2024-08-17 21:30:46
 * @ Modified by: @godot42
 * @ Modified time: 2024-08-18 04:28:14
 * @ Description:
 */



#pragma once
#include "__microshit_api_hazel.h"
#include "hazel/core/timestep.h"
#include "hazel/core/uuid.h"
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
class IDComponent;


using Entity_t = entt::entity;

class HAZEL_API Scene
{

    friend class Entity;
    friend class SceneHierarchyPanel;
    friend class SceneSerializer;


  protected:
    entt::registry m_Registry;
    uint32_t       m_ViewportWidth  = 0;
    uint32_t       m_ViewportHeight = 0;
    b2World       *m_PhysicsWorld   = nullptr;


  public:
    Scene();
    virtual ~Scene() = default;

    static Ref<Scene> Copy(Ref<Scene> scene);

    void OnRuntimeStart();
    void OnRuntimeStop();

    void OnUpdateEditor(Timestep ts, EditorCamera &camera);
    void OnUpdateRuntime(Timestep ts);
    void OnViewportResize(uint32_t w, uint32_t h);

  public:
    Entity GetPrimaryCameraEntity();

    Entity CreateEntity(const std::string &name);
    Entity CreateEntityWithUUID(UUID uuid, const std::string &name);
    void   DuplicateEntity(Entity entity);


    void DestroyEntity(Entity entity);

    uint32_t GetViewportWidth() const { return m_ViewportWidth; }
    uint32_t GetViewportHeight() const { return m_ViewportHeight; }
};



} // namespace hazel