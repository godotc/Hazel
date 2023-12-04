#pragma once

#include "hazel/core/timestep.h"
#include "scene_camera.h"
#include "scriptable_entity.h"
#include <functional>
#include <string>

namespace hazel {
struct TagComponent {
    std::string Tag;

    TagComponent()                     = default;
    TagComponent(const TagComponent &) = default;
    TagComponent(const std::string &tag) : Tag(tag) {}
};

struct TransformComponent {
    glm::mat4 Tranform{1.f};

    TransformComponent() {}
    TransformComponent(const TransformComponent &) = default;
    TransformComponent(const glm::mat4 &transform) : Tranform(transform) {}

    operator glm::mat4 &() { return Tranform; }
    operator const glm::mat4 &() const { return Tranform; }
};


struct SpriteRendererComponent {
    glm::vec4 Color{1, 1, 1, 1};


    SpriteRendererComponent() {}
    SpriteRendererComponent(const SpriteRendererComponent &) = default;
    SpriteRendererComponent(const glm::vec4 &color) : Color(color) {}

    operator glm::vec4 &() { return Color; }
    operator const glm::vec4 &() const { return Color; }
};


struct CameraComponent {
    SceneCamera Camera;
    bool        bPrimary          = true; // TODO: mvoe to scene
    bool        bFixedAspectRatio = false;

    CameraComponent()                        = default;
    CameraComponent(const CameraComponent &) = default;
};

struct NativeScriptComponent {
    ScriptableEntity *Instance = nullptr;

    using InstantiateScript = ScriptableEntity *();
    InstantiateScript *InstantiateScriptFunc;

    using DestoryScript = void(NativeScriptComponent *);
    DestoryScript *DestoryScriptFunc;

    template <class T>
    void Bind()
    {
        InstantiateScriptFunc = []() { return static_cast<ScriptableEntity *>(new T()); };
        DestoryScriptFunc     = [](NativeScriptComponent *nsc) { delete nsc->Instance; nsc->Instance= nullptr; };
    }
};

} // namespace hazel