#pragma once

#include "glm/ext/quaternion_float.hpp"
#include "hazel/core/base.h"
#include "hazel/core/timestep.h"
#include "hazel/renderer/texture.h"
#include "hazel/scene/scriptable_entity.h"
#include "scene_camera.h"
#include <string>

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

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
        glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
        // glm::mat4 rotation =
        //     glm::rotate(glm::mat4(1.f), Rotation.x, {1, 0, 0}) *
        //     glm::rotate(glm::mat4(1.f), Rotation.y, {0, 1, 0}) *
        //     glm::rotate(glm::mat4(1.f), Rotation.z, {0, 0, 1});

        return glm::translate(glm::mat4(1.f), Translation) *
               rotation *
               glm::scale(glm::mat4(1.f), Scale);
    }
};


struct SpriteRendererComponent {
    glm::vec4      Color{1, 1, 1, 1};
    Ref<Texture2D> Texture;
    float          TilingFactor = 1.0f;

    SpriteRendererComponent() {}
    SpriteRendererComponent(const SpriteRendererComponent &) = default;
    SpriteRendererComponent(const glm::vec4 &color) : Color(color) {}

    operator glm::vec4 &() { return Color; }
    operator const glm::vec4 &() const { return Color; }
};


struct CameraComponent {
    SceneCamera Camera;
    bool        bPrimary          = true; // TODO: move to scene
    bool        bFixedAspectRatio = false;

    CameraComponent()                        = default;
    CameraComponent(const CameraComponent &) = default;
};

struct NativeScriptComponent {
    ScriptableEntity *Instance = nullptr;

    using InstantiateScript = ScriptableEntity *();
    InstantiateScript *InstantiateScriptFunc;

    using DestroyScript = void(NativeScriptComponent *);
    DestroyScript *DestroyScriptFunc;

    template <class T>
    void Bind()
    {
        InstantiateScriptFunc = []() { return static_cast<ScriptableEntity *>(new T()); };
        DestroyScriptFunc     = [](NativeScriptComponent *nsc) { delete nsc->Instance; nsc->Instance= nullptr; };
    }
};


struct Rigidbody2DComponent {
    enum class EBodyType
    {
        Static = 0,
        Dynamic,
        Kinematic
    };
    EBodyType Type           = EBodyType::Static;
    bool      bFixedRotation = false;

    // Storage for runtime
    void *RuntimeBody = nullptr;

    Rigidbody2DComponent()                             = default;
    Rigidbody2DComponent(const Rigidbody2DComponent &) = default;
};

struct BoxCollider2DComponent {
    glm::vec2 Offset = {0.0f, 0.0f};
    glm::vec2 Size   = {0.5f, 0.5f};

    float Density              = 1.0f; // 冲力
    float Friction             = 0.5f; // 摩擦力
    float Restitution          = 0.0f; // ???
    float RestitutionThreshold = 0.5f; //

    // Storage for runtime
    void *RuntimeFixture = nullptr;

    BoxCollider2DComponent()                               = default;
    BoxCollider2DComponent(const BoxCollider2DComponent &) = default;
};

} // namespace hazel