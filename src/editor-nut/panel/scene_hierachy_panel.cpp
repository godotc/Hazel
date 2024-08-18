#include "hz_pch.h"

#include "entt/entity/fwd.hpp"

#include "glm/ext/vector_float3.hpp"


#include "glm/trigonometric.hpp"
#include "hazel/scene/scene.h"
#include "hazel/scene/scene_camera.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "glm/gtc/type_ptr.hpp"
#include "scene_hierachy_panel.h"
#include <string>

#include "editor_layer.h"
#include "hazel/scene/component.h"
#include "utils/file.h"
#include "utils/path.h"



namespace hazel {

namespace imgui = ImGui;


#pragma region

// NOTICE!!: make sure each Vec3Control has different label
void draw_vec3_Control(const std::string &label, glm::vec3 &values, float reset_value = 0.f, float column_width = 100.f)
{
    imgui::PushID(label.c_str());

    imgui::Columns(2);

    imgui::SetColumnWidth(0, column_width);
    // imgui::PushTextWrapPos(0.f);
    imgui::Text("%s", label.c_str());
    // imgui::PopTextWrapPos();
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
    imgui::PopID(); // make each vec3 are different drag float here
}


template <class T, class UIFunction>
void draw_component(const std::string &name, Entity entity, UIFunction ui_func)
{

    const auto tree_node_flags = ImGuiTreeNodeFlags_DefaultOpen |
                                 ImGuiTreeNodeFlags_AllowItemOverlap |
                                 ImGuiTreeNodeFlags_SpanAvailWidth |
                                 ImGuiTreeNodeFlags_FramePadding |
                                 ImGuiTreeNodeFlags_Framed;

    if (entity.HasComponent<T>())
    {
        auto  &component                = entity.GetComponent<T>();
        ImVec2 content_region_available = imgui::GetContentRegionAvail();

        imgui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
        float line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.f;
        imgui::Separator();

        bool bOpen = ImGui::TreeNodeEx((void *)typeid(T).hash_code(), tree_node_flags, "%s", name.c_str());

        imgui::PopStyleVar();

        imgui::SameLine(content_region_available.x - line_height * 0.5f);

        if (imgui::Button("+", ImVec2{line_height, line_height})) {
            imgui::OpenPopup("ComponentSettings");
        }

        bool bRemoveComponent = false;

        if (imgui::BeginPopup("ComponentSettings"))
        {
            if (imgui::MenuItem("Remove Component")) {
                bRemoveComponent = true;
            }
            imgui::EndPopup();
        }

        if (bOpen)
        {
            ui_func(component);

            imgui::TreePop();
        }

        if (bRemoveComponent) {
            entity.RemoveComponent<T>();
        }
    }
}
#pragma endregion

SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene> &scene)
{
    m_Context = scene;
}

void SceneHierarchyPanel::SetContext(const Ref<Scene> &scene)
{
    m_Context = scene;
    SetSelection({});
}

void SceneHierarchyPanel::OnImGuiRender()
{
    if (ImGui::Begin("Scene Hierarchy"))
    {
        auto &entities = m_Context->m_Registry.storage<entt::entity>();

#ifndef NDebug
        // TODO: Why cannot to get the size of entities here?
        int32_t size = entities.size();
#endif
        for (auto &entt : entities) {
            if (auto entity = Entity{entt, m_Context.get()}) {
                DrawEntityNode(entity);
            }
        };

        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
            SetSelection({});
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



    static bool bPropertiesWindowOpened = true;
    ImGui::Begin("Properties", &bPropertiesWindowOpened, ImGuiWindowFlags_DockNodeHost | ImGuiWindowFlags_NoCollapse);
    if (bPropertiesWindowOpened) {
        if (m_Selection) {
            DrawComponents(m_Selection);
        }
        ImGui::End();
    }
}

void SceneHierarchyPanel::DrawEntityNode(Entity entity)
{
    bool bEntityDeleted = false;
    // right click this entity
    if (imgui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverExistingPopup)) {
        if (imgui::MenuItem("Delete Entity")) {
            bEntityDeleted = true;
        }
        ImGui::EndPopup();
    }

    auto &tag = entity.GetComponent<TagComponent>().Tag;

    auto flags =
        // entity.HasChildren? ImGuiTreeNodeFlags_OpenOnArrow :0 |
        (entity == m_Selection ? ImGuiTreeNodeFlags_Selected : 0) |
        ImGuiTreeNodeFlags_SpanAvailWidth;

    bool bOpened = ImGui::TreeNodeEx((void *)(uint64_t)(uint32_t)entity, flags, "%s", tag.c_str());


    // left click
    if (ImGui::IsItemClicked()) {
        m_Selection = entity;
    }


    if (bOpened)
    {
        // treenode's children here
        // ...
        ImGui::TreePop();
    }

    // delete this at last avoid issues
    if (bEntityDeleted) {
        m_Context->DestroyEntity(entity);
        SetSelection({});
    }
}


void SceneHierarchyPanel::DrawComponents(Entity entity)
{
    if (entity.HasComponent<TagComponent>()) {
        auto &tag = entity.GetComponent<TagComponent>().Tag;
        if (tag.size() < 256) {
            tag.resize(256);
        }
        ImGui::InputText("Tag", tag.data(), tag.size());
    }

    imgui::SameLine();
    imgui::PushItemWidth(-1);
    if (imgui::Button("Add Component")) {
        imgui::OpenPopup("AddComponent");
    }

    if (imgui::BeginPopup("AddComponent"))
    {
        // if (!m_Selection.HasComponent<CameraComponent>()) {
        if (imgui::MenuItem("Camera")) {
            m_Selection.AddComponent<CameraComponent>();
            imgui::CloseCurrentPopup();
        }
        // }

        if (!m_Selection.HasComponent<SpriteRendererComponent>()) {
            if (imgui::MenuItem("Sprite Renderer")) {
                m_Selection.AddComponent<SpriteRendererComponent>();
                imgui::CloseCurrentPopup();
            }
        }
        if (!m_Selection.HasComponent<Rigidbody2DComponent>())
        {
            if (ImGui::MenuItem("RigidBody 2D"))
            {
                m_Selection.AddComponent<Rigidbody2DComponent>();
                ImGui::CloseCurrentPopup();
            }
        }
        if (!m_Selection.HasComponent<BoxCollider2DComponent>())
        {
            if (ImGui::MenuItem("Box Collider 2D"))
            {
                m_Selection.AddComponent<BoxCollider2DComponent>();
                ImGui::CloseCurrentPopup();
            }
        }
        if (!m_Selection.HasComponent<CircleRendererComponent>())
        {
            if (ImGui::MenuItem("Circle Renderer")) {
                m_Selection.AddComponent<CircleRendererComponent>();
                ImGui::CloseCurrentPopup();
            }
        }

        imgui::EndPopup();
    }
    imgui::PopItemWidth();


    draw_component<TransformComponent>("Transform ", entity, [](auto &tc) {
        draw_vec3_Control("Translation", tc.Translation);
        glm::vec3 rotation = glm::degrees(tc.Rotation);
        draw_vec3_Control(" Rotation", rotation);
        tc.Rotation = glm::radians(rotation);
        draw_vec3_Control(" Scale", tc.Scale);
    });


    draw_component<CameraComponent>("Camera Component", entity, [](auto &component) {
        auto &camera = component.Camera;

        // TODO & FIXME: change to true will clean all other camera's bPrimary value
        imgui::Checkbox("Primary", &component.bPrimary);
        imgui::Checkbox("FixedAspectRatio", &component.bFixedAspectRatio);


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
                    component.Camera.SetProjectionType(SceneCamera::ProjectionType(i));
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
                    float perspective_fov = glm::degrees(camera.GetPerspectiveFovy());
                    if (imgui::DragFloat("VerticalFov", &perspective_fov)) {
                        camera.SetPerspectiveFovy(glm::radians(perspective_fov));
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
                    float ortho_near_size = camera.GetOrthographicNear();
                    if (imgui::DragFloat("Near", &ortho_near_size)) {
                        camera.SetOrthographicNear(ortho_near_size);
                    }
                    float ortho_far_size = camera.GetOrthographicFar();
                    if (imgui::DragFloat("Far", &ortho_far_size)) {
                        camera.SetOrthographicFar(ortho_far_size);
                    }
                    break;
                }
            };
        }
    });


    draw_component<SpriteRendererComponent>("Sprite Renderer", entity, [](SpriteRendererComponent &component) {
        imgui::ColorEdit4("Color", glm::value_ptr(component.Color));
        imgui::DragFloat("Tiling Factor", &component.TilingFactor, 0.01f, 0.0f, 100.0f);

        ImGui::ImageButton(component.Texture ? (ImTextureID)component.Texture->GetTextureID() : nullptr, {64, 64}, {0, 1}, {1, 0});
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                const wchar_t *path_str = (const wchar_t *)payload->Data;
                auto           path     = EditorLayer::DefaultAssetsDirectory() / path_str;
                if (utils::File::is_image(path)) {
                    auto texture = Texture2D::Create(path.string());
                    if (texture->IsLoaded()) {
                        component.Texture = texture;
                    }
                    else {
                        HZ_CORE_WARN("Failed to load texture {}", path.string());
                    }
                }
                else {
                    HZ_CORE_WARN("File {} is not an image", path.string());
                }
            }
            ImGui::EndDragDropTarget();
        }
    });

    draw_component<CircleRendererComponent>("CircleRenderer", entity, [](CircleRendererComponent &component) {
        imgui::ColorEdit4("Color", glm::value_ptr(component.Color));
        imgui::DragFloat("Thickness", &component.Thickness, 0.01f, 0.0f, 1.0f);
        imgui::DragFloat("Fade", &component.Fade, 0.01f, 0.0f, 1.0f);
        // imgui::DragFloat("Radius", &component.Radius, 0.01f, 0.0f, 100.0f);
    });



    draw_component<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto &component) {
        static const char *body_type_str[]   = {"Static", "Dynamic", "Kinematic"};
        const char        *cur_body_type_str = body_type_str[(int)component.Type];
        if (ImGui::BeginCombo("Body Type", cur_body_type_str))
        {
            for (int i = 0; i < 2; i++)
            {
                bool bSelected = cur_body_type_str == body_type_str[i];
                if (ImGui::Selectable(body_type_str[i], bSelected))
                {
                    cur_body_type_str = body_type_str[i];
                    component.Type    = (Rigidbody2DComponent::EBodyType)i;
                }

                if (bSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        ImGui::Checkbox("Fixed Rotation", &component.bFixedRotation);
    });

    draw_component<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto &component) {
        ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
        ImGui::DragFloat2("Size", glm::value_ptr(component.Offset));
        ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
    });
}



void SceneHierarchyPanel::SetSelection(Entity new_selection)
{
    if (m_Selection != new_selection) {
        m_LastSelection = m_Selection;
        m_Selection     = new_selection;
    }
}
} // namespace hazel
