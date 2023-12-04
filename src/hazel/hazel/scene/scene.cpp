#include "glm/ext/vector_float4.hpp"
#include "hazel/renderer/camera.h"
#include "hazel/renderer/render_2d.h"
#include "hazel/scene/component.h"
#include "hazel/scene/entity.h"
#include "hazel/scene/scriptable_entity.h"

namespace hazel {



Entity Scene::CreateEntity(const std::string &name)
{
    auto entity = Entity{m_Registry.create(), this};
    entity.AddComponent<TransformComponent>();
    auto &tag = entity.AddComponent<TagComponent>();
    tag.Tag   = name.empty() ? "Entity" : name;

    return entity;
}

Scene::Scene()
{
}


void Scene::OnUpdate(Timestep ts)
{
    // Update scripts
    {
        m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto &ns_comp) {
            if (!ns_comp.Instance) {
                ns_comp.InstantiateFuncton();
                ns_comp.Instance->m_Entity = Entity{entity, this};
                ns_comp.OnCreateFunction(ns_comp.Instance);
            }
            ns_comp.OnUpdateFuncton(ns_comp.Instance, ts);
        });
    }



    // render 2d scene
    Camera    *main_camera = nullptr;
    glm::mat4 *transfrom   = nullptr;
    {
        auto view = m_Registry.view<TransformComponent, CameraComponent>();
        for (auto ent : view) {
            auto &&[tranf, cam] = view.get(ent);
            if (cam.bPrimary) {
                main_camera = &cam.Camera;
                transfrom   = &tranf.Tranform;
                break;
            }
        }
    }

    if (main_camera && transfrom)
    {
        Render2D::BeginScene(*main_camera, *transfrom);

        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group) {
            auto [tranf, color] = group.get<TransformComponent, SpriteRendererComponent>(entity);
            Render2D::DrawQuad(tranf, 1.f, color);
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

} // namespace hazel