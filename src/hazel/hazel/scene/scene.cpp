#include "scene.h"
#include "entt/entity/fwd.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float4.hpp"
#include "hazel/renderer/render_2d.h"
#include "hazel/scene/component.h"
namespace hazel {



entt::entity Scene::CreateEntity()
{
    return m_Registry.create();
}

Scene::Scene()
{
}

void Scene::OnUpdate(Timestep ts)
{
    auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
    for (auto entity : group) {
        auto [tranf, color] = group.get<TransformComponent, SpriteRendererComponent>(entity);
        Render2D::DrawQuad(tranf, 1.f, {1, 0, 0, 1});
    }
}

} // namespace hazel