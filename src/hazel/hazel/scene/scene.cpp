#include "entt/core/iterator.hpp"
#include "entt/entity/fwd.hpp"
#include "glm/ext/vector_float4.hpp"
#include "hazel/core/base.h"
#include "hazel/renderer/camera.h"
#include "hazel/renderer/render_2d.h"
#include "hazel/scene/component.h"
#include "hazel/scene/entity.h"

namespace hazel {



Entity Scene::CreateEntity(const std::string &name)
{
    auto entity = Entity{m_Registry.create(), this};
    entity.AddComponent<TransformComponent>();
    auto &tag = entity.AddComponent<TagComponent>();
    tag.Tag   = name.empty() ? "Entity" : name;

    return entity;
}

void Scene::DestoryEntity(Entity entity)
{
    m_Registry.destroy(entity);
}


Scene::Scene()
{
}


void Scene::OnUpdateEditor(Timestep ts, EditorCamera &camera)
{
    Render2D::BeginScene(camera);

    auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
    for (entt::entity entity : group) {
        auto [tranf, color] = group.get<TransformComponent, SpriteRendererComponent>(entity);
        Render2D::DrawSprite(tranf.GetTransform(), color, int(entity));
    }

    Render2D::EndScene();
}

void Scene::OnUpdateRumtime(Timestep ts)
{
    // Update scripts TODO: in runtime
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


    // render 2d scene
    Camera   *main_camera = nullptr;
    glm::mat4 transfrom;
    {
        auto view = m_Registry.view<TransformComponent, CameraComponent>();
        for (auto ent : view) {
            auto [tranf, cam] = view.get(ent);
            if (cam.bPrimary) {
                main_camera = &cam.Camera;
                transfrom   = tranf.GetTransform();
                break;
            }
        }
    }

    // FIXME: the scale of camera should not affects the scene scale

    if (main_camera)
    {
        Render2D::BeginScene(*main_camera, transfrom);

        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group) {
            auto [tranf, color] = group.get<TransformComponent, SpriteRendererComponent>(entity);
            Render2D::DrawQuad(tranf.GetTransform(), 1.f, color);
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


template <class T>
void Scene::OnComponentAdd(Entity entity, T &component) { HZ_CORE_ASSERT(false, "failed"); }

template <>
void Scene::OnComponentAdd<TransformComponent>(Entity entity, TransformComponent &component)
{
}

template <>
void Scene::OnComponentAdd<CameraComponent>(Entity entity, CameraComponent &component)
{
    if (m_ViewportWidth > 0 && m_ViewportHeight > 0) {
        component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
    }
}

template <>
void Scene::OnComponentAdd<TagComponent>(Entity entity, TagComponent &component)
{
}

template <>
void Scene::OnComponentAdd<SpriteRendererComponent>(Entity entity, SpriteRendererComponent &component)
{
}

template <>
void Scene::OnComponentAdd<NativeScriptComponent>(Entity entity, NativeScriptComponent &component) {}

} // namespace hazel