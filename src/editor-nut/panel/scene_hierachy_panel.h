#pragma once

#include "hazel/core/base.h"
#include "hazel/scene/entity.h"
#include "hazel/scene/scene.h"



namespace hazel {

class SceneHierarchyPanel
{
    Ref<Scene> m_Context;
    Entity     m_LastSelection;
    Entity     m_Selection;

  public:
    SceneHierarchyPanel() = default;
    SceneHierarchyPanel(const Ref<Scene> &scene);

    void         SetContext(const Ref<Scene> &scene);
    const Entity GetSelectedEntity() const { return m_Selection; }
    void         SetSelection(Entity new_selection);


    void OnImGuiRender();

  private:
  private:
    void DrawEntityNode(Entity entity);
    void DrawComponents(Entity entity);

    void UI_AddComponents();
};


} // namespace hazel