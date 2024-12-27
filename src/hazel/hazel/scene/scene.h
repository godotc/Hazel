/**
 * @ Author: godot42
 * @ Create Time: 2024-08-17 21:30:46
 * @ Modified by: @godot42
 * @ Modified by: @godot42
 * @ Modified time: 2024-12-28 05:20:10
 */



#pragma once
#include "__microshit_api_hazel.h"
#include "hazel/core/timestep.h"
#include "hazel/core/uuid.h"
#include "hazel/scene/component.h"
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
    Entity* m_RuntimeCameraEntity;


  public:
    Scene();
    virtual ~Scene();

    static Ref<Scene> Copy(Ref<Scene> scene);

    void OnRuntimeStart();
    void OnRuntimeStop();

    void OnSimulationStart();
    void OnSimulationStop();

    void OnUpdateEditor(Timestep ts, EditorCamera &camera);
    void OnUpdateSimulation(Timestep ts, EditorCamera &camera);
    void OnUpdateRuntime(Timestep ts);

    void OnViewportResize(uint32_t w, uint32_t h);

    template <typename... Component>
    auto GetAllEntitiesWith()
    {
        return m_Registry.view<Component...>();
    }

  public:
    Entity GetPrimaryCameraEntity();

    Entity CreateEntity(const std::string &name);
    Entity CreateEntityWithUUID(UUID uuid, const std::string &name);
    void   DuplicateEntity(Entity entity);


    void DestroyEntity(Entity entity);

    uint32_t GetViewportWidth() const { return m_ViewportWidth; }
    uint32_t GetViewportHeight() const { return m_ViewportHeight; }

  private:
    void RenderScene(const EditorCamera &camera);

    void UpdatePhysics2D(Timestep ts);

    void OnPhysics2DStart();
    void OnPhysics2DStop();
};


template <class T>
struct only_copy_component {
    static constexpr bool value = !(std::is_same_v<T, IDComponent> || std::is_same_v<T, TagComponent>);
};
using TCopyComponentTypes = sref::filter<TComponentTypes, only_copy_component>;

template <class T>
struct without_mutable_component {
    static constexpr bool value = !(std::is_same_v<T, IDComponent> || std::is_same_v<T, TagComponent>);
};

// The IDComponent and TagComponent are created at the same time when a new entity is created
// So we should block them when adding new components
using TWithoutMutableComponentTypes = sref::filter<TComponentTypes, without_mutable_component>;


#pragma endregion


} // namespace hazel