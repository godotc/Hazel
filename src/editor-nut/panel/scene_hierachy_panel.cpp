#include "hz_pch.h"

#include "glm/ext/vector_float3.hpp"
#include "imgui_internal.h"


#include "glm/trigonometric.hpp"
#include "hazel/scene/scene.h"
#include "hazel/scene/scene_camera.h"

#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "scene_hierachy_panel.h"
#include <string>



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
        });

        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
            m_Selection = {};
        }


        // right click on black space
        if (imgui::BeginPopupContextWindow(nullptr, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
            if (imgui::MenuItem("Create Empty Entity")) {
                m_Context->CreateEntity("Empty Entity");
            }
            ImGui::EndPopup();
        }

        ImGui::End();
    }



    if (ImGui::Begin("Properties")) {
        if (m_Selection) {
            DrawComponents(m_Selection);

            if (imgui::Button("Add Component")) {
                imgui::OpenPopup("AddComponent");
            }
            if (imgui::BeginPopup("AddComponent")) {
                if (imgui::MenuItem("Camera")) {
                    m_Selection.AddComponent<CameraComponent>();
                }
                if (imgui::MenuItem("Sprite Renderer")) {
                    m_Selection.AddComponent<SpriteRendererComponent>();
                }
                imgui::EndPopup();
            }
        }
        ImGui::End();
    }
}

void SceneHierarchyPanel::DrawEntityNode(Entity entity)
{
    bool bEntityDeleted = false;

    auto &tag = entity.GetComponent<TagComponent>().Tag;

    auto flags =
        // entity.HasChildren? ImGuiTreeNodeFlags_OpenOnArrow :0 |
        (entity == m_Selection ? ImGuiTreeNodeFlags_Selected : 0);

    if (ImGui::TreeNodeEx((void *)(uint64_t)(uint32_t)entity, flags, tag.c_str())) {

        if (ImGui::IsItemClicked()) {
            m_Selection = entity;
        }

        // right click this entity
        if (imgui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonRight)) {
            if (imgui::MenuItem("Delete Entity")) {
                bEntityDeleted = true;
            }
            ImGui::EndPopup();
        }

        // treenode's children here
        // ...
        ImGui::TreePop();
    }

    // delete this at last avoid issues
    if (bEntityDeleted) {
        m_Context->DestoryEntity(entity);
        m_Selection = {};
    }
}

// NOTICE!!: make sure each Vec3Control has different label
static void DrawVec3Control(const std::string &label, glm::vec3 &values, float reset_value = 0.f, float column_width = 100.f)
{
    imgui::PushID(label.c_str());


    imgui::Columns(2);
    imgui::SetColumnWidth(0, column_width);
    imgui::Text(label.c_str());
    imgui::NextColumn();

    imgui::PushMultiItemsWidths(3, imgui::CalcItemWidth());
    imgui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});
    {

        float  line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.f;
        ImVec2 button_size = {line_height + 3.f, line_height};

        {
            imgui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8, 0.1, 0.1, 1});
            imgui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9, 0.2, 0.2, 1});
            imgui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8, 0.1, 0.15, 1});
            if (imgui::Button("X", button_size)) {
                values.x = reset_value;
            }
            imgui::PopStyleColor(3);
            imgui::SameLine();
            imgui::DragFloat("##X", &values.x, 0.1f, 0.f, 0.f, "%.2f");
            imgui::PopItemWidth();
        }
        imgui::SameLine();
        {
            imgui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1, 0.7, 0.1, 1});
            imgui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2, 0.9, 0.2, 1});
            imgui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.15, 0.8, 0.11, 1});
            if (imgui::Button("Y", button_size)) {
                values.y = reset_value;
            }
            imgui::PopStyleColor(3);
            imgui::SameLine();
            imgui::DragFloat("##Y", &values.y, 0.1f, 0.f, 0.f, "%.2f");
            imgui::PopItemWidth();
        }
        imgui::SameLine();
        {
            imgui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1, 0.1, 0.8, 1});
            imgui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2, 0.2, 0.8, 1});
            imgui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.15, 0.1, 0.8, 1});
            if (imgui::Button("Z", button_size)) {
                values.z = reset_value;
            }
            imgui::PopStyleColor(3);
            imgui::SameLine();
            imgui::DragFloat("##Z", &values.z, 0.1f, 0.f, 0.f, "%.2f");
            imgui::PopItemWidth();
        }
    }
    imgui::PopStyleVar();

    imgui::Columns(1);
    imgui::PopID(); // make each vec3 are different drag flaot here
}


void SceneHierarchyPanel::DrawComponents(Entity entity)
{


    const auto tree_node_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap;

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
        bool bOpen = ImGui::TreeNodeEx((void *)typeid(TransformComponent).hash_code(),
                                       tree_node_flags,
                                       "Transform");


        if (bOpen)
        {
            auto &tc = entity.GetComponent<TransformComponent>();
            DrawVec3Control("Translation", tc.Translation);
            glm::vec3 rotation = glm::degrees(tc.Rotation);
            DrawVec3Control(" Rotation", rotation);
            tc.Rotation = glm::radians(rotation);
            DrawVec3Control(" Scale", tc.Scale);
            ImGui::TreePop();
        }
    }
    if (entity.HasComponent<CameraComponent>())
    {
        if (ImGui::TreeNodeEx(
                (void *)typeid(CameraComponent).hash_code(),
                tree_node_flags,
                "Camera"))
        {

            auto &cc     = entity.GetComponent<CameraComponent>();
            auto &camera = cc.Camera;

            // TODO & FIXME: change to true will clean all other camera's bPrimeary value
            imgui::Checkbox("Primary", &cc.bPrimary);
            imgui::Checkbox("FixedAspecRatio", &cc.bFixedAspectRatio);


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
    if (entity.HasComponent<SpriteRendererComponent>())
    {
        bool bOpen = ImGui::TreeNodeEx((void *)typeid(SpriteRendererComponent).hash_code(),
                                       ImGuiTreeNodeFlags_DefaultOpen,
                                       "Sprite Renderer");
        imgui::SameLine(imgui::GetWindowWidth() - 20.f);
        if (imgui::Button("+", ImVec2{20.f, 20.f})) {
            imgui::OpenPopup("ComponentSettings");
        }
        bool bRemoveComponent = false;
        if (imgui::BeginPopup("ComponentSettings")) {
            if (imgui::MenuItem("Remove Component")) {
                bRemoveComponent = true;
            }
            imgui::EndPopup();
        }

        if (bOpen)
        {
            auto &src = entity.GetComponent<SpriteRendererComponent>();
            imgui::ColorEdit4("Color", glm::value_ptr(src.Color));
            imgui::TreePop();
        }
        if (bRemoveComponent) {
            entity.RemoveComponent<SpriteRendererComponent>();
        }
    }
}

} // namespace hazel
