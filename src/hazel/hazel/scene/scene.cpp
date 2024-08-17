/**
 * @ Author: godot42
 * @ Create Time: 2024-08-15 22:17:08
 * @ Modified by: @godot42
 * @ Modified time: 2024-08-18 03:27:45
 * @ Description:
 */



#include "scene.h"
#include "hazel/core/uuid.h"

#include "hazel/core/base.h"
#include "hazel/renderer/camera.h"
#include "hazel/renderer/render_2d.h"
#include "hazel/scene/component.h"
#include "hazel/scene/entity.h"

#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_world.h"
#include <cstdint>


#include "hazel/scene/scriptable_entity.h"



namespace hazel {
template <class Component>
static void copy_component(entt::registry &dst, const entt::registry &src, const std::unordered_map<UUID, entt::entity> &entt_map);

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

void Scene::DestroyEntity(Entity entity)
{
    m_Registry.destroy(entity);
}


Scene::Scene()
{
}

template <class Component>
static void copy_component(entt::registry &dst, const entt::registry &src, const std::unordered_map<UUID, entt::entity> &entt_map)
{
    for (auto e : src.view<Component>())
    {
        UUID uuid = src.get<IDComponent>(e).ID;
        HZ_CORE_ASSERT(entt_map.contains(uuid));
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

    // except id and tag component
    // TODO: static reflection typelist
    copy_component<TransformComponent>(dst_scene_registry, src_scene_registry, entt_map);
    copy_component<SpriteRendererComponent>(dst_scene_registry, src_scene_registry, entt_map);
    copy_component<CameraComponent>(dst_scene_registry, src_scene_registry, entt_map);
    copy_component<Rigidbody2DComponent>(dst_scene_registry, src_scene_registry, entt_map);
    copy_component<BoxCollider2DComponent>(dst_scene_registry, src_scene_registry, entt_map);
    copy_component<NativeScriptComponent>(dst_scene_registry, src_scene_registry, entt_map);


    return new_scene;
}

void Scene::OnRuntimeStart()
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
    }
}

void Scene::OnRuntimeStop()
{
    delete m_PhysicsWorld;
    m_PhysicsWorld = nullptr;
}


void Scene::OnUpdateEditor(Timestep ts, EditorCamera &camera)
{
    Render2D::BeginScene(camera);

    auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
    for (entt::entity entity : group) {
        auto [transf, color] = group.get<TransformComponent, SpriteRendererComponent>(entity);
        Render2D::DrawSprite(transf.GetTransform(), color, int(entity));
    }

    Render2D::EndScene();
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
    {
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
        }
    }


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

        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group) {
            auto [transf, color] = group.get<TransformComponent, SpriteRendererComponent>(entity);
            Render2D::DrawQuad(transf.GetTransform(), 1.f, color);
        }

        Render2D::EndScene();
    }
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