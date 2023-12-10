#include "glm/trigonometric.hpp"
#include "hazel/scene/scene.h"
#include "hazel/scene/scene_camera.h"
#include "hz_pch.h"

#include "glm/matrix.hpp"

#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "hazel/scene//component.h"
#include "hazel/scene/entity.h"
#include "imgui.h"
#include "scene_hierachy_panel.h"
#include <array>
#include <cstdint>
#include <cstring>



namespace hazel {

namespace imgui = ImGui;

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
    if (entity.HasComponent<CameraComponent>())
    {
        if (ImGui::TreeNodeEx(
                (void *)typeid(CameraComponent).hash_code(),
                ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen,
                "Camera"))
        {

            auto &cc     = entity.GetComponent<CameraComponent>();
            auto &camera = cc.Camera;

            // TODO & FIXME: move the update primary for all cameras here
            imgui::Checkbox("Primary", &cc.bPrimary);


            auto old_projection_type = camera.GetProjectionType();

            const char *projection_type_strings[] = {"Perspective", "Orthographic"};
            const char *current_projection_type   = projection_type_strings[(int)old_projection_type];
            if (imgui::BeginCombo("Projection", current_projection_type))
            {
                for (int i = 0; i < 2; ++i)
                {
                    bool bSelected = current_projection_type = projection_type_strings[i];
                    if (imgui::Selectable(projection_type_strings[i], bSelected)) {
                        current_projection_type = projection_type_strings[i];
                        cc.Camera.SetProjectionType(SceneCamera::ProjectionType(i));
                    }
                    if (bSelected) {
                        imgui::SetItemDefaultFocus();
                    }
                }

                imgui::EndCombo();
            }

            if (old_projection_type != camera.GetProjectionType()) {
                switch (camera.GetProjectionType())
                {
                    case SceneCamera::ProjectionType::Perspective:
                    {
                        float perspective_fov = glm::degrees(camera.GetPerspectiveFOV());
                        if (imgui::DragFloat("VerticalFov", &perspective_fov)) {
                            camera.SetPerspectiveFOV(glm::radians(perspective_fov));
                        }
                        float perspective_near = camera.GetPerspectiveNear();
                        if (imgui::DragFloat("Near", &perspective_near)) {
                            camera.SetPerspectiveNear(perspective_near);
                        }
                        float perspective_far = camera.GetPerspectiveFar();
                        if (imgui::DragFloat("Far", &perspective_far)) {
                            camera.SetPerspectiveFar(perspective_far);
                        }
                        break;
                    }
                    case SceneCamera::ProjectionType::Orthographic:
                    {
                        float ortho_size = camera.GetOrthographicSize();
                        if (imgui::DragFloat("Size", &ortho_size)) {
                            camera.SetOrthographicSize(ortho_size);
                        }
                        float ortho_near_size = camera.GetOrthographicNearClip();
                        if (imgui::DragFloat("Near", &ortho_near_size)) {
                            camera.SetOrthographicNearClip(ortho_near_size);
                        }
                        float ortho_far_size = camera.GetOrthographicFarClip();
                        if (imgui::DragFloat("Far", &ortho_far_size)) {
                            camera.SetOrthographicFarClip(ortho_far_size);
                        }
                        break;
                    }
                };
            }
            imgui::TreePop();
        }
    }
}

} // namespace hazel