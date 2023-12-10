#pragma once


#include "hazel/core/base.h"
#include "hazel/scene/component.h"
#include "hazel/scene/scene.h"
#include "imgui.h"
namespace hazel {

class SceneHierarchyPanel
{
    friend class Scene;

    Ref<Scene> m_Context;
    Entity     m_Selection;

  public:
    SceneHierarchyPanel() = default;
    SceneHierarchyPanel(const Ref<Scene> &scene);

    void SetContext(const Ref<Scene> &scene);
    void OnImGuiRender();


  private:
    void DrawEntityNode(Entity entity);
};


} // namespace hazel