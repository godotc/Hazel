/**
 * @ Author: godot42
 * @ Create Time: 2024-08-15 22:17:08
 * @ Modified by: @godot42
 * @ Modified time: 2024-12-15 02:52:50
 * @ Description:
 */



#include "scene.h"
#include "box2d/b2_collision.h"
#include "glm/ext/matrix_transform.hpp"
#include "hazel/core/uuid.h"

#include "hazel/core/base.h"
#include "hazel/renderer/camera.h"
#include "hazel/renderer/render_2d.h"
#include "hazel/scene/component.h"
#include "hazel/scene/entity.h"

#include "box2d/b2_body.h"
#include "box2d/b2_circle_shape.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_world.h"
#include <cstdint>



#include "hazel/scene/scriptable_entity.h"
#include "hazel/sref/typelist.hpp"



namespace hazel {


template <class Component>
static void copy_component(entt::registry &dst, const entt::registry &src, const std::unordered_map<UUID, entt::entity> &entt_map);
template <class Component>
static void copy_component_if_exist(Entity dst, Entity src)
{
    if (src && src.HasComponent<Component>()) {
        dst.AddOrReplaceComponent<Component>(static_cast<Component &>(src.GetComponent<Component>()));
    }
}

static b2BodyType Rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::EBodyType body_type)
{
    switch (body_type)
    {
        case Rigidbody2DComponent::EBodyType::Static:
            return b2_staticBody;
        case Rigidbody2DComponent::EBodyType::Dynamic:
            return b2_dynamicBody;
        case Rigidbody2DComponent::EBodyType::Kinematic:
            return b2_kinematicBody;
    }

    HZ_CORE_ASSERT(false, "Unknown body type");
    return b2_staticBody;
}



Entity Scene::CreateEntity(const std::string &name)
{
    return CreateEntityWithUUID(hazel::UUID{}, name);
}


Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string &name)
{
    auto entity = Entity{m_Registry.create(), this};
    entity.AddComponent<IDComponent>(uuid);
    entity.AddComponent<TransformComponent>();

    auto &tag = entity.AddComponent<TagComponent>();
    tag.Tag   = name.empty() ? "Entity" : name;

    return entity;
}

void Scene::DuplicateEntity(Entity entity)
{
    if (!entity) {
        return;
    }

    std::string new_name = entity.GetName() + "(copy)";
    HZ_CORE_INFO("Duplicate entity: {0}", new_name);
    auto new_entity = CreateEntity(new_name);

    // except id and tag component
    sref::foreach_types<TCopyComponentTypes>([&new_entity, &entity](auto type_val) {
        copy_component_if_exist<decltype(type_val)>(new_entity, entity);
    });
}

void Scene::DestroyEntity(Entity entity)
{
    m_Registry.destroy(entity);
}

void Scene::RenderScene(const EditorCamera &camera)
{
    Render2D::BeginScene(camera);

    {
        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (entt::entity entity : group) {
            auto [transf, color] = group.get<TransformComponent, SpriteRendererComponent>(entity);
            Render2D::DrawSprite(transf.GetTransform(), color, int(entity));
        }
    }
    {
        auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
        for (entt::entity entity : view) {
            auto [transf, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
            Render2D::DrawCircle(transf.GetTransform(), circle.Color, circle.Thickness, circle.Fade, int(entity));
        }
    }

    Render2D::EndScene();
}

void Scene::UpdatePhysics2D(Timestep ts)
{
    // Physics
    static const int32_t velocity_iterations = 6;
    static const int32_t position_iterations = 2;
    m_PhysicsWorld->Step(ts, velocity_iterations, position_iterations);

    auto view = m_Registry.view<Rigidbody2DComponent>();
    for (auto e : view)
    {
        Entity entity{e, this};

        auto &transform = entity.GetComponent<TransformComponent>();
        auto &rb2d      = entity.GetComponent<Rigidbody2DComponent>();

        const b2Body *body      = (b2Body *)rb2d.RuntimeBody;
        const auto   &position  = body->GetPosition();
        transform.Translation.x = position.x;
        transform.Translation.y = position.y;
        transform.Rotation.z    = body->GetAngle();

        if (entity.HasComponent<BoxCollider2DComponent>()) {
            BoxCollider2DComponent &bc2d    = entity.GetComponent<BoxCollider2DComponent>();
            const b2Fixture        *fixture = (b2Fixture *)bc2d.RuntimeFixture;
            bc2d.Density                    = fixture->GetDensity();
            bc2d.Friction                   = fixture->GetFriction();
            bc2d.Restitution                = fixture->GetRestitution();
            bc2d.RestitutionThreshold       = fixture->GetRestitutionThreshold();
        }
        if (entity.HasComponent<CircleCollider2DComponent>()) {
            CircleCollider2DComponent &cc2d    = entity.GetComponent<CircleCollider2DComponent>();
            const b2Fixture           *fixture = (b2Fixture *)cc2d.RuntimeFixture;
            cc2d.Density                       = fixture->GetDensity();
            cc2d.Friction                      = fixture->GetFriction();
            cc2d.Restitution                   = fixture->GetRestitution();
            cc2d.RestitutionThreshold          = fixture->GetRestitutionThreshold();
        }
    }
}

void Scene::OnPhysics2DStart()
{
    m_PhysicsWorld = new b2World({0.0f, -9.8f});

    auto view = m_Registry.view<Rigidbody2DComponent>();
    for (auto e : view)
    {
        Entity entity{e, this};
        auto  &transf = entity.GetComponent<TransformComponent>();
        auto  &rb2d   = entity.GetComponent<Rigidbody2DComponent>();

        b2BodyDef body_def;
        body_def.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);
        body_def.position.Set(transf.Translation.x, transf.Translation.y);
        body_def.angle = transf.Rotation.z;

        b2Body *body = m_PhysicsWorld->CreateBody(&body_def);
        body->SetFixedRotation(rb2d.bFixedRotation);
        rb2d.RuntimeBody = body;

        if (entity.HasComponent<BoxCollider2DComponent>())
        {
            auto &bc2d = entity.GetComponent<BoxCollider2DComponent>();

            b2PolygonShape shape;
            // half-width and half-height
            shape.SetAsBox(bc2d.Size.x * transf.Scale.x, bc2d.Size.y * transf.Scale.y);

            b2FixtureDef fixture_def;
            fixture_def.shape                = &shape;
            fixture_def.density              = bc2d.Density;
            fixture_def.friction             = bc2d.Friction;
            fixture_def.restitution          = bc2d.Restitution;
            fixture_def.restitutionThreshold = bc2d.RestitutionThreshold;

            b2Fixture *fixture  = body->CreateFixture(&fixture_def);
            bc2d.RuntimeFixture = fixture;
        }

        if (entity.HasComponent<CircleCollider2DComponent>())
        {
            auto &cc2d = entity.GetComponent<CircleCollider2DComponent>();

            b2CircleShape shape;
            shape.m_p      = {cc2d.Offset.x, cc2d.Offset.y};
            shape.m_radius = cc2d.Radius * transf.Scale.x;

            b2FixtureDef fixture_def;
            fixture_def.shape                = &shape;
            fixture_def.density              = cc2d.Density;
            fixture_def.friction             = cc2d.Friction;
            fixture_def.restitution          = cc2d.Restitution;
            fixture_def.restitutionThreshold = cc2d.RestitutionThreshold;

            b2Fixture *fixture  = body->CreateFixture(&fixture_def);
            cc2d.RuntimeFixture = fixture;
        }
    }
}

void Scene::OnPhysics2DStop()
{
    delete m_PhysicsWorld;
    m_PhysicsWorld = nullptr;
}


Scene::Scene()
{
}

Scene::~Scene()
{
    delete m_PhysicsWorld;
}

template <class Component>
static void copy_component(entt::registry &dst, const entt::registry &src, const std::unordered_map<UUID, entt::entity> &entt_map)
{
    for (auto e : src.view<Component>())
    {
        UUID uuid = src.get<IDComponent>(e).ID;
        HZ_CORE_ASSERT(entt_map.contains(uuid), "UUID not found in entity map");
        auto dst_entt_id = entt_map.at(uuid);

        auto &src_component = src.get<Component>(e);
        dst.emplace_or_replace<Component>(dst_entt_id, src_component);
    }
}

Ref<Scene> Scene::Copy(Ref<Scene> scene)
{
    auto new_scene = CreateRef<Scene>();

    new_scene->m_ViewportHeight = scene->m_ViewportHeight;
    new_scene->m_ViewportWidth  = scene->m_ViewportWidth;

    std::unordered_map<UUID, entt::entity> entt_map;

    const auto &src_scene_registry = scene->m_Registry;
    auto       &dst_scene_registry = new_scene->m_Registry;

    // create entities in new scene
    auto id_view = src_scene_registry.view<IDComponent>();
    for (auto e : id_view)
    {
        UUID        uuid = src_scene_registry.get<IDComponent>(e).ID;
        const auto &name = src_scene_registry.get<TagComponent>(e).Tag;

        auto entity = new_scene->CreateEntityWithUUID(uuid, name);
        entt_map.insert({uuid, entity});
        // entt_map[uuid] = entity;
    }

    // id and tag component have been created, now copy the rest
    sref::foreach_types<TCopyComponentTypes>([&dst_scene_registry, &src_scene_registry, entt_map](auto type_val) {
        copy_component<decltype(type_val)>(dst_scene_registry, src_scene_registry, entt_map);
    });


    return new_scene;
}

void Scene::OnRuntimeStart()
{
    OnPhysics2DStart();
}

void Scene::OnRuntimeStop()
{
    OnPhysics2DStop();
}

void Scene::OnSimulationStart()
{
    OnPhysics2DStart();
}

void Scene::OnSimulationStop()
{
    OnPhysics2DStop();
}



void Scene::OnUpdateEditor(Timestep ts, EditorCamera &camera)
{
    RenderScene(camera);
}

void Scene::OnUpdateRuntime(Timestep ts)
{
    // Update scripts
    {
        m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto &ns_comp) {
            // TODO: Move to Scene:BeginPlay()
            if (!ns_comp.Instance) {
                ns_comp.Instance           = ns_comp.InstantiateScriptFunc();
                ns_comp.Instance->m_Entity = Entity{entity, this};
                ns_comp.Instance->OnCreate();
            }
            ns_comp.Instance->OnUpdate(ts);
        });
    }


    // Physics
    UpdatePhysics2D(ts);


    // render 2d scene
    Camera   *main_camera = nullptr;
    glm::mat4 transform;
    {
        auto view = m_Registry.view<TransformComponent, CameraComponent>();
        for (auto ent : view) {
            auto [transf, cam] = view.get(ent);
            if (cam.bPrimary) {
                main_camera = &cam.Camera;
                transform   = transf.GetTransform();
                break;
            }
        }
    }

    // FIXME: the scale of camera should not affects the scene scale

    if (main_camera)
    {
        Render2D::BeginScene(*main_camera, transform);

        // quad
        {
            auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
            for (entt::entity entity : group) {
                auto [transf, color] = group.get<TransformComponent, SpriteRendererComponent>(entity);
                Render2D::DrawSprite(transf.GetTransform(), color, int(entity));
            }
        }
        // circle
        {
            auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
            for (entt::entity entity : view) {
                auto [transf, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
                Render2D::DrawCircle(transf.GetTransform(), circle.Color, circle.Thickness, circle.Fade, int(entity));
            }
        }

        Render2D::EndScene();
    }
}

void Scene::OnUpdateSimulation(Timestep ts, EditorCamera &camera)
{
    UpdatePhysics2D(ts);
    RenderScene(camera);
}

void Scene::OnViewportResize(uint32_t w, uint32_t h)
{
    m_ViewportWidth  = w;
    m_ViewportHeight = h;

    // resize
    auto view = m_Registry.view<CameraComponent>();
    for (auto ent : view) {
        auto &camera_component = view.get<CameraComponent>(ent);
        if (!camera_component.bFixedAspectRatio)
        {
            camera_component.Camera.SetViewportSize(w, h);
        }
    }
}

Entity Scene::GetPrimaryCameraEntity()
{
    auto view = m_Registry.view<CameraComponent>();
    for (auto entity : view) {
        const auto &camera = view.get<CameraComponent>(entity);
        if (camera.bPrimary) {
            return Entity(entity, this);
        }
    }

    return {};
}



} // namespace hazel