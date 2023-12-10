#include "hz_pch.h"

#include "hazel/scene//component.h"
#include "hazel/scene/entity.h"
#include "imgui.h"
#include "scene_hierachy_panel.h"
#include <cstdint>



namespace hazel {

SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene> &scene)
{
    m_Context = scene;
}

void SceneHierarchyPanel::SetContext(const Ref<Scene> &scene)
{
    m_Context = scene;
}

void SceneHierarchyPanel::OnImGuiRender()
{
    if (ImGui::Begin("Scene Hierachy"))
    {
        m_Context->m_Registry.each([this](auto entt) {
            auto entity = Entity{entt, m_Context.get()};
            DrawEntityNode(entity);
            auto &tc = entity.GetComponent<TagComponent>();
            ImGui::Text("%s", tc.Tag);
        });
        ImGui::End();
    }
}

void SceneHierarchyPanel::DrawEntityNode(Entity entity)
{
    auto &tag = entity.GetComponent<TagComponent>().Tag;

    auto flags =
        ImGuiTreeNodeFlags_OpenOnArrow |
        (entity == m_Selection ? ImGuiTreeNodeFlags_Selected : 0);

    bool bOpened = ImGui::TreeNodeEx((void *)(uint64_t)(uint32_t)entity, flags, tag.c_str());

    if (ImGui::IsItemClicked()) {
        m_Selection = entity;
    }
    if (bOpened) {
        // treenode's children here
        // ...
        bool bOpened = ImGui::TreeNodeEx((void *)(uint64_t)(uint32_t) "whatever", flags, tag.c_str());
        if (bOpened) {
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
}

} // namespace hazel