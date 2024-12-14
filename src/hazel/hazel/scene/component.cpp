/**
 * @ Author: godot42
 * @ Create Time: 2024-11-13 17:47:25
 * @ Modified by: @godot42
 * @ Modified time: 2024-12-15 00:51:26
 * @ Description:
 */


#include "component.h"
#include "hazel/scene/scriptable_entity.h"

namespace hazel {

void NativeScriptComponent::DestroyScriptFuncImpl(NativeScriptComponent *nsc)
{
    delete nsc->Instance;
    nsc->Instance = nullptr;
}

void CameraComponent::OnComponentAddedImpl(const Scene *scene)
{
    auto w = scene->GetViewportWidth();
    auto h = scene->GetViewportHeight();
    if (w > 0 && h > 0) {
        Camera.SetViewportSize(w, h);
    }
}

} // namespace hazel