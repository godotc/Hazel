#include "glm/matrix.hpp"
#include "hz_pch.h"

#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "hazel/scene//component.h"
#include "hazel/scene/entity.h"
#include "imgui.h"
#include "scene_hierachy_panel.h"
#include <cstdint>
#include <cstring>



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

        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
            m_Selection = {};
        }

        ImGui::End();
    }



    if (ImGui::Begin("Properties")) {
        if (m_Selection) {
            DrawComponents(m_Selection);
        }
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
        ImGui::TreePop();
    }
}

void SceneHierarchyPanel::DrawComponents(Entity entity)
{
    if (entity.HasComponent<TagComponent>()) {
        auto &tag = entity.GetComponent<TagComponent>().Tag;
        char  buf[256];
        memset(buf, 0, sizeof(buf));
        memcpy(buf, tag.data(), sizeof(buf));
        if (ImGui::InputText("Tag", buf, sizeof(buf))) {
            tag = buf;
        }
    }
    if (entity.HasComponent<TransformComponent>()) {
        if (ImGui::TreeNodeEx((void *)typeid(TransformComponent).hash_code(),
                              ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen,
                              "Transform"))
        {

            auto     &transform     = entity.GetComponent<TransformComponent>().Tranform;
            glm::mat4 buf_transform = transform;
            glm::vec3 position      = transform[3];
            glm::vec3 rotation      = transform[2];
            glm::vec3 scale         = glm::transpose(transform)[3];


            ImGui::DragFloat3("Position", glm::value_ptr(transform[3]));
            ImGui::DragFloat3("Rotation", glm::value_ptr(rotation));
            ImGui::DragFloat3("Scale", glm::value_ptr(scale));
            ImGui::TreePop();
        }
    }
}

} // namespace hazel