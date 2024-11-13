/**
 * @ Author: godot42
 * @ Create Time: 2024-08-21 22:28:05
 * @ Modified by: @godot42
 * @ Modified time: 2024-11-13 19:30:52
 * @ Description:
 */

#pragma once

#include "glm/ext/quaternion_float.hpp"
#include "hazel/renderer/texture.h"
#include "scene_camera.h"
#include <string>

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "hazel/core/uuid.h"
#include "hazel/sref/typelist.hpp"


namespace hazel {

struct ScriptableEntity;
struct Scene;

template <class T>
struct Component {

    virtual ~Component() = default;

  public:
    void        OnComponentAdded(const Scene *scene) { static_cast<T *>(this)->OnComponentAddedImpl(scene); }
    static const char *GetComponentName() { return T::GetComponentNameImpl(); }

  protected:
    virtual void        OnComponentAddedImpl(const Scene *scene) = 0;

#define GENERATED_COMPONENT_BODY(cls) \
    static const char *GetComponentNameImpl(){ return #cls; }
};

struct IDComponent : public Component<IDComponent> {
    GENERATED_COMPONENT_BODY(IDComponent)

    UUID ID;


    IDComponent() = default;
    IDComponent(UUID id) : ID(id) {}
    IDComponent(const IDComponent &) = default;

    void OnComponentAddedImpl(const Scene *scene) override {}
};

struct TagComponent : public Component<TagComponent> {
    GENERATED_COMPONENT_BODY(TagComponent)

    std::string Tag;

    TagComponent()                     = default;
    TagComponent(const TagComponent &) = default;
    TagComponent(const std::string &tag) : Tag(tag) {}

    void OnComponentAddedImpl(const Scene *scene) override {}
};

struct TransformComponent : public Component<TransformComponent> {
    GENERATED_COMPONENT_BODY(TransformComponent)
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

    void OnComponentAddedImpl(const Scene *scene) override {}
};


struct SpriteRendererComponent : public Component<SpriteRendererComponent> {
    GENERATED_COMPONENT_BODY(SpriteRendererComponent)

    glm::vec4      Color{1, 1, 1, 1};
    Ref<Texture2D> Texture;
    float          TilingFactor = 1.0f;

    SpriteRendererComponent() {}
    SpriteRendererComponent(const SpriteRendererComponent &) = default;
    SpriteRendererComponent(const glm::vec4 &color) : Color(color) {}

    operator glm::vec4 &() { return Color; }
    operator const glm::vec4 &() const { return Color; }

    void OnComponentAddedImpl(const Scene *scene) override {}
};




struct CircleRendererComponent : public Component<CircleRendererComponent> {
    GENERATED_COMPONENT_BODY(CircleRendererComponent)

    glm::vec4 Color{1, 1, 1, 1};
    // float     Radius    = 0.5f;
    float Thickness = 1.f;
    float Fade      = 0.005f;


    CircleRendererComponent() {}
    CircleRendererComponent(const CircleRendererComponent &) = default;

    void OnComponentAddedImpl(const Scene *scene) override {}
};


struct CameraComponent : public Component<CameraComponent> {
    GENERATED_COMPONENT_BODY(CameraComponent)

    SceneCamera Camera;
    bool        bPrimary          = true; // TODO: move to scene
    bool        bFixedAspectRatio = false;

    CameraComponent()                        = default;
    CameraComponent(const CameraComponent &) = default;

    void OnComponentAddedImpl(const Scene *scene) override;
};


struct Rigidbody2DComponent : public Component<Rigidbody2DComponent> {
    GENERATED_COMPONENT_BODY(Rigidbody2DComponent)

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

    void OnComponentAddedImpl(const Scene *scene) override {}
};

struct BoxCollider2DComponent : public Component<BoxCollider2DComponent> {
    GENERATED_COMPONENT_BODY(BoxCollider2DComponent)

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

    void OnComponentAddedImpl(const Scene *scene) override {}
};

struct CircleCollider2DComponent : public Component<CircleCollider2DComponent> {
    GENERATED_COMPONENT_BODY(CircleCollider2DComponent)

    glm::vec2 Offset = {0.0f, 0.0f};
    float     Radius = 0.5f;

    float Density              = 1.0f; // 冲力
    float Friction             = 0.5f; // 摩擦力
    float Restitution          = 0.0f; // ???
    float RestitutionThreshold = 0.5f; //

    // Storage for runtime
    void *RuntimeFixture = nullptr;

    CircleCollider2DComponent()                                  = default;
    CircleCollider2DComponent(const CircleCollider2DComponent &) = default;

    void OnComponentAddedImpl(const Scene *scene) override {}
};


struct NativeScriptComponent : public Component<NativeScriptComponent> {
    GENERATED_COMPONENT_BODY(NativeScriptComponent)

    ScriptableEntity *Instance = nullptr;

    using InstantiateScript = ScriptableEntity *();
    InstantiateScript *InstantiateScriptFunc;

    using DestroyScript = void(NativeScriptComponent *);
    DestroyScript *DestroyScriptFunc;

    template <class T>
    void Bind()
    {
        InstantiateScriptFunc = []() { return static_cast<ScriptableEntity *>(new T()); };
        DestroyScriptFunc     = DestroyScriptFuncImpl;
    }

    static void DestroyScriptFuncImpl(NativeScriptComponent *nsc);

    void OnComponentAddedImpl(const Scene *scene) override {}
};


using TComponentTypes = sref::type_list<
    IDComponent,
    TagComponent,
    TransformComponent,
    SpriteRendererComponent,
    CircleRendererComponent,
    CameraComponent,
    Rigidbody2DComponent,
    BoxCollider2DComponent,
    CircleCollider2DComponent,
    NativeScriptComponent>;


} // namespace hazel
