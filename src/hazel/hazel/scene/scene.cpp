#include "scene.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float4.hpp"
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

Scene::Scene()
{
}


void Scene::OnUpdate(Timestep ts)
{
    auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
    for (auto entity : group) {
        auto [tranf, color] = group.get<TransformComponent, SpriteRendererComponent>(entity);
        Render2D::DrawQuad(tranf, 1.f, color);
    }
}

} // namespace hazel