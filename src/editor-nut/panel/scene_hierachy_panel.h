#pragma once

#include "hazel/core/base.h"
#include "hazel/scene/entity.h"
#include "hazel/scene/scene.h"



namespace hazel {

void DrawVec3Control(const std::string &label, glm::vec3 &values, float reset_value, float column_width);
class SceneHierarchyPanel
{
    friend class Scene;

    Ref<Scene> m_Context;
    Entity     m_Selection;

  public:
    SceneHierarchyPanel() = default;
    SceneHierarchyPanel(const Ref<Scene> &scene);

    void   SetContext(const Ref<Scene> &scene);
    Entity GetSelectedEntity() const { return m_Selection; }

    void OnImGuiRender();

  private:
    void DrawEntityNode(Entity entity);
    void DrawComponents(Entity entity);
};


} // namespace hazel