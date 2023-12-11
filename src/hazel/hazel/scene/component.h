#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "hazel/core/timestep.h"
#include "scene_camera.h"
#include "scriptable_entity.h"
#include <string>

namespace hazel {
struct TagComponent {
    std::string Tag;

    TagComponent()                     = default;
    TagComponent(const TagComponent &) = default;
    TagComponent(const std::string &tag) : Tag(tag) {}
};

struct TransformComponent {
    glm ::vec3 Translation = {0.f, 0.f, 0.f};
    glm ::vec3 Rotation    = {0.f, 0.f, 0.f};
    glm ::vec3 Scale       = {1.f, 1.f, 1.f};

    TransformComponent() {}
    TransformComponent(const TransformComponent &) = default;
    TransformComponent(const glm::vec3 &translation) : Translation(translation) {}

    glm::mat4 GetTransform() const
    {
        glm::mat4 rotation =
            glm::rotate(glm::mat4(1.f), Rotation.x, {1, 0, 0}) *
            glm::rotate(glm::mat4(1.f), Rotation.y, {0, 1, 0}) *
            glm::rotate(glm::mat4(1.f), Rotation.z, {0, 0, 1});

        return glm::translate(glm::mat4(1.f), Translation) *
               rotation *
               glm::scale(glm::mat4(1.f), Scale);
    }
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