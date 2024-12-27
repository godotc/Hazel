//
/*
 * @ Author: godot42
 * @ Create Time: 2024-11-23 01:27:43
 * @ Modified by: @godot42
 * @ Modified time: 2024-12-28 04:03:08
 * @ Description:
 */


#include "hazel/sref/typelist.hpp"
#include "hz_pch.h"
//

#include "entt/entity/fwd.hpp"

#include "glm/ext/vector_float3.hpp"
#include "hazel/scene/scene_camera.h"

#include "component.h"
#include "hazel/core/log.h"
#include "scene.h"


#include "hazel/core/base.h"
#include "hazel/scene/entity.h"
#include "scene_serializer.h"
#include "utils/path.h"
#include "yaml-cpp/emitter.h"
#include "yaml-cpp/emittermanip.h"
#include "yaml-cpp/node/node.h"
#include "yaml-cpp/node/parse.h"



#include <cerrno>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <yaml-cpp/yaml.h>

namespace YAML {
template <>
struct convert<glm::vec3> {
    static Node encode(const glm::vec3 &rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        return node;
    }
    static bool decode(const Node &node, glm::vec3 &rhs)
    {
        if (node.IsSequence() || node.size() == 3) {
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
        return false;
    }
};

template <>
struct convert<glm::vec4> {
    static Node encode(const glm::vec4 &rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        node.push_back(rhs.w);
        return node;
    }
    static bool decode(const Node &node, glm::vec4 &rhs)
    {
        if (node.IsSequence() || node.size() == 4) {
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
        return false;
    }
};

template <>
struct convert<glm::vec2> {

    static Node encode(const glm::vec2 &rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        return node;
    }
    static bool decode(const Node &node, glm::vec2 &rhs)
    {
        if (!node.IsSequence() || node.size() != 2) {
            return false;
        }
        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        return true;
    }
};

} // namespace YAML

namespace hazel {



YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec2 &v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq;
    out << v.x << v.y;
    out << YAML::EndSeq;
    return out;
}

YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec3 &v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq;
    out << v.x << v.y << v.z;
    out << YAML::EndSeq;
    return out;
}
YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec4 &v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq;
    out << v.x << v.y << v.z << v.w;
    out << YAML::EndSeq;
    return out;
}


static std::string RigidBody2DBodyTypeToString(Rigidbody2DComponent::EBodyType bodyType)
{
    switch (bodyType)
    {
        case Rigidbody2DComponent::EBodyType::Static:
            return "Static";
        case Rigidbody2DComponent::EBodyType::Dynamic:
            return "Dynamic";
        case Rigidbody2DComponent::EBodyType::Kinematic:
            return "Kinematic";
    }

    HZ_CORE_ASSERT(false, "Unknown body type");
    return {};
}

static Rigidbody2DComponent::EBodyType RigidBody2DBodyTypeFromString(const std::string &bodyTypeString)
{
    if (bodyTypeString == "Static")
        return Rigidbody2DComponent::EBodyType::Static;
    if (bodyTypeString == "Dynamic")
        return Rigidbody2DComponent::EBodyType::Dynamic;
    if (bodyTypeString == "Kinematic")
        return Rigidbody2DComponent::EBodyType::Kinematic;

    HZ_CORE_ASSERT(false, "Unknown body type");
    return Rigidbody2DComponent::EBodyType::Static;
}

SceneSerializer::SceneSerializer(const Ref<Scene> &scene)
{
    m_Scene = scene;
}

struct YAMLSerializerCompatHelper : hazel::SerializerCompatHelper {

    YAML::Emitter &m_Emitter;

    explicit YAMLSerializerCompatHelper(YAML::Emitter &emitter) : m_Emitter(emitter) {}
    void *Get() override { return &m_Emitter; }
};


static void SerializeEntity(YAML::Emitter &out, Entity &entity)
{
    HZ_CORE_ASSERT(entity.HasComponent<IDComponent>());

    out << YAML::BeginMap;
    out << YAML::Key << "Entity" << YAML::Value << (uint64_t)entity.GetUUID();

    if (entity.HasComponent<TagComponent>())
    {
        auto &comp = entity.GetComponent<TagComponent>();
        out << YAML::Key << "TagComponent";
        out << YAML::BeginMap;
        {
            out << YAML::Key << "Tag" << YAML::Value << comp.Tag.c_str();
        }
        out << YAML::EndMap;
    }
    if (entity.HasComponent<TransformComponent>()) {
        auto &comp = entity.GetComponent<TransformComponent>();
        out << YAML::Key << "TransformComponent";
        out << YAML::BeginMap;
        {
            out << YAML::Key << "Translation" << YAML::Value << comp.Translation;
            out << YAML::Key << "Rotation" << YAML::Value << comp.Rotation;
            out << YAML::Key << "Scale" << YAML::Value << comp.Scale;
        }
        out << YAML::EndMap;
    }
    if (entity.HasComponent<CameraComponent>()) {
        auto &comp = entity.GetComponent<CameraComponent>();


        out << YAML::Key << "CameraComponent";
        out << YAML::BeginMap;
        {
            out << YAML::Key << "bFixedAspectRatio" << YAML::Value << comp.bFixedAspectRatio;
            out << YAML::Key << "bPrimary" << YAML::Value << comp.bPrimary;

            out << YAML::Key << "Camera";
            out << YAML::BeginMap;
            {
                out << YAML::Key << "ProjectionType" << YAML::Value << (int)comp.Camera.GetProjectionType();

                out << YAML::Key << "PerspectiveFovy" << YAML::Value << comp.Camera.GetPerspectiveFovy();
                out << YAML::Key << "PerspectiveFar" << YAML::Value << comp.Camera.GetPerspectiveFar();
                out << YAML::Key << "PerspectiveNear" << YAML::Value << comp.Camera.GetPerspectiveNear();

                out << YAML::Key << "OrthographicSize" << YAML::Value << comp.Camera.GetOrthographicSize();
                out << YAML::Key << "OrthographicNear" << YAML::Value << comp.Camera.GetOrthographicNear();
                out << YAML::Key << "OrthographicFar" << YAML::Value << comp.Camera.GetOrthographicFar();
            }
            out << YAML::EndMap;
        }
        out << YAML::EndMap;
    }
    if (entity.HasComponent<SpriteRendererComponent>()) {
        auto &comp = entity.GetComponent<SpriteRendererComponent>();

        out << YAML::Key << "SpriteRendererComponent";
        out << YAML::BeginMap;
        {
            // get the color as a key and the value to serialize
            out << YAML::Key << "Color" << YAML::Value << comp.Color;
            if (comp.Texture) {
                auto texture_path = std::filesystem::relative(comp.Texture->GetPath());
                out << YAML::Key << "TexturePath" << YAML::Value << texture_path.string();
            }
            out << YAML::Key << "TillingFactor" << YAML::Value << comp.TilingFactor;
        }
        out << YAML::EndMap;
    }
    if (entity.HasComponent<CircleRendererComponent>())
    {
        out << YAML::Key << "CircleRendererComponent";
        out << YAML::BeginMap; // CircleRendererComponent

        auto &circle_render_component = entity.GetComponent<CircleRendererComponent>();
        out << YAML::Key << "Color" << YAML::Value << circle_render_component.Color;
        out << YAML::Key << "Thickness" << YAML::Value << circle_render_component.Thickness;
        out << YAML::Key << "Fade" << YAML::Value << circle_render_component.Fade;

        out << YAML::EndMap; // CircleRendererComponent
    }
    if (entity.HasComponent<NativeScriptComponent>()) {
        auto &comp = entity.GetComponent<NativeScriptComponent>();

        out << YAML::Key << "NativeScriptComponent";

        out << YAML::BeginMap;
        {
            // out << YAML::Key << "Color" << YAML::Value << comp.;
        }
        out << YAML::EndMap;
    }
    if (entity.HasComponent<Rigidbody2DComponent>()) {
        out << YAML::Key << "Rigidbody2DComponent";
        out << YAML::BeginMap; // Rigidbody2DComponent

        auto &rb2dComponent = entity.GetComponent<Rigidbody2DComponent>();
        out << YAML::Key << "BodyType" << YAML::Value << RigidBody2DBodyTypeToString(rb2dComponent.Type);
        out << YAML::Key << "bFixedRotation" << YAML::Value << rb2dComponent.bFixedRotation;

        out << YAML::EndMap; // Rigidbody2DComponent
    }

    if (entity.HasComponent<BoxCollider2DComponent>())
    {
        out << YAML::Key << "BoxCollider2DComponent";
        out << YAML::BeginMap; // BoxCollider2DComponent

        auto &bc2dComponent = entity.GetComponent<BoxCollider2DComponent>();
        out << YAML::Key << "Offset" << YAML::Value << bc2dComponent.Offset;
        out << YAML::Key << "Size" << YAML::Value << bc2dComponent.Size;
        out << YAML::Key << "Density" << YAML::Value << bc2dComponent.Density;
        out << YAML::Key << "Friction" << YAML::Value << bc2dComponent.Friction;
        out << YAML::Key << "Restitution" << YAML::Value << bc2dComponent.Restitution;
        out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc2dComponent.RestitutionThreshold;

        out << YAML::EndMap; // BoxCollider2DComponent
    }

    if (entity.HasComponent<CircleCollider2DComponent>())
    {
        out << YAML::Key << "CircleCollider2DComponent";
        out << YAML::BeginMap; // CircleCollider2DComponent
        auto &cc2d_comp = entity.GetComponent<CircleCollider2DComponent>();

        out << YAML::Key << "Offset" << YAML::Value << cc2d_comp.Offset;
        out << YAML::Key << "Size" << YAML::Value << cc2d_comp.Radius;
        out << YAML::Key << "Density" << YAML::Value << cc2d_comp.Density;
        out << YAML::Key << "Friction" << YAML::Value << cc2d_comp.Friction;
        out << YAML::Key << "Restitution" << YAML::Value << cc2d_comp.Restitution;
        out << YAML::Key << "RestitutionThreshold" << YAML::Value << cc2d_comp.RestitutionThreshold;

        out << YAML::EndMap;
    }



    out << YAML::EndMap;
}

void SceneSerializer::Serialize(const std::string &filepath)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    {
        out << YAML::Key << "Scene" << YAML::Value << "TODO";
        out << YAML::Key << "Entities";
        out << YAML::BeginSeq;
        {
            for (auto entity_id : m_Scene->m_Registry.storage<Entity_t>())
            {
                Entity entity{entity_id, m_Scene.get()};
                if (!entity) {
                    HZ_WARN("An entity is invalid!");
                    return;
                }
                // YAMLSerializerCompatHelper helper{out};
                SerializeEntity(out, entity);
            };
        }
        out << YAML::EndSeq;
    }
    out << YAML::EndMap;

    std::ofstream f(filepath);
    f << out.c_str();
}

void SceneSerializer::SerializeRuntime(const std::string &filepath)
{
    // TODO
    HZ_CORE_ASSERT(false);
}

bool SceneSerializer::Deserialize(const std::string &filepath)
{
    namespace fs                       = std::filesystem;
    std::filesystem::path project_root = utils::ProjectRoot();
    auto                  current      = fs::current_path();
    if (!fs::equivalent(current, utils::ProjectRoot())) {
        project_root = current;
    }

    YAML::Node data;

    try {
        data = YAML::LoadFile(filepath);
    }
    catch (YAML::ParserException e) {
        HZ_CORE_ERROR("Failed to parse YAML file {}: error: {}", filepath, e.what());
        return false;
    }

    auto scene = data["Scene"];
    if (!scene) {
        return false;
    }

    std::string scene_name = scene.as<std::string>();
    HZ_CORE_TRACE("Deserializing scene '{}'", scene_name);

    auto entities = data["Entities"];
    if (entities) {
        for (auto entity : entities) {

            uint64_t uuid = entity["Entity"].as<uint64_t>();

            std::string name;
            if (auto tag_component = entity["TagComponent"]) {
                name = tag_component["Tag"].as<std::string>();
            }

            HZ_CORE_TRACE("Deserialized entity with ID = {}, name = {}", uuid, name);
            Entity deserialized_entity = m_Scene->CreateEntityWithUUID(uuid, name);

            if (auto transform_component = entity["TransformComponent"]) {
                auto &tc       = deserialized_entity.GetOrAddComponent<TransformComponent>();
                tc.Translation = transform_component["Translation"].as<glm::vec3>();
                tc.Rotation    = transform_component["Rotation"].as<glm::vec3>();
                tc.Scale       = transform_component["Scale"].as<glm::vec3>();
            }

            if (const auto &camera_component = entity["CameraComponent"]) {
                auto &cc = deserialized_entity.AddComponent<CameraComponent>();

                cc.bPrimary          = camera_component["bPrimary"].as<bool>();
                cc.bFixedAspectRatio = camera_component["bFixedAspectRatio"].as<bool>();

                const auto camera_props = camera_component["Camera"];

                cc.Camera.SetProjectionType(static_cast<SceneCamera::ProjectionType>(camera_props["ProjectionType"].as<int>()));

                cc.Camera.SetOrthographicNear(camera_props["OrthographicNear"].as<float>());
                cc.Camera.SetOrthographicFar(camera_props["OrthographicFar"].as<float>());
                cc.Camera.SetOrthographicSize(camera_props["OrthographicSize"].as<float>());

                cc.Camera.SetPerspectiveFar(camera_props["PerspectiveFar"].as<float>());
                cc.Camera.SetPerspectiveNear(camera_props["PerspectiveNear"].as<float>());
                cc.Camera.SetPerspectiveFovy(camera_props["PerspectiveFovy"].as<float>());
            }

            if (const auto &sprite_renderer_component = entity["SpriteRendererComponent"]) {
                auto &tc = deserialized_entity.AddComponent<SpriteRendererComponent>();
                tc.Color = sprite_renderer_component["Color"].as<glm::vec4>();
                if (const auto &node = sprite_renderer_component["TexturePath"]) {
                    const auto &path = node.as<std::string>();
                    if (!path.empty()) {
                        auto resolved_path = std::filesystem::relative(path, project_root);
                        tc.Texture         = Texture2D::Create(path);
                    }
                }
            }

            auto circle_render_component = entity["CircleRendererComponent"];
            if (circle_render_component)
            {
                auto &crc     = deserialized_entity.AddComponent<CircleRendererComponent>();
                crc.Color     = circle_render_component["Color"].as<glm::vec4>();
                crc.Thickness = circle_render_component["Thickness"].as<float>();
                crc.Fade      = circle_render_component["Fade"].as<float>();
            }

            if (const auto rigid_body_2d_component = entity["Rigidbody2DComponent"])
            {
                auto &rb2d          = deserialized_entity.AddComponent<Rigidbody2DComponent>();
                rb2d.Type           = RigidBody2DBodyTypeFromString(rigid_body_2d_component["BodyType"].as<std::string>());
                rb2d.bFixedRotation = rigid_body_2d_component["bFixedRotation"].as<bool>();
            }

            if (const auto box_collider_2d_component = entity["BoxCollider2DComponent"])
            {
                auto &bc2d = deserialized_entity.AddComponent<BoxCollider2DComponent>();

                bc2d.Offset               = box_collider_2d_component["Offset"].as<glm::vec2>();
                bc2d.Size                 = box_collider_2d_component["Size"].as<glm::vec2>();
                bc2d.Density              = box_collider_2d_component["Density"].as<float>();
                bc2d.Friction             = box_collider_2d_component["Friction"].as<float>();
                bc2d.Restitution          = box_collider_2d_component["Restitution"].as<float>();
                bc2d.RestitutionThreshold = box_collider_2d_component["RestitutionThreshold"].as<float>();
            }

            if (const auto circle_collider_2d_component = entity["CircleCollider2DComponent"])
            {
                auto &cc2d = deserialized_entity.AddComponent<CircleCollider2DComponent>();

                cc2d.Offset               = circle_collider_2d_component["Offset"].as<glm::vec2>();
                cc2d.Radius               = circle_collider_2d_component["Size"].as<float>();
                cc2d.Density              = circle_collider_2d_component["Density"].as<float>();
                cc2d.Friction             = circle_collider_2d_component["Friction"].as<float>();
                cc2d.Restitution          = circle_collider_2d_component["Restitution"].as<float>();
                cc2d.RestitutionThreshold = circle_collider_2d_component["RestitutionThreshold"].as<float>();
            }
        }
    }

    return true;
}

bool SceneSerializer::DeserializeRuntime(const std::string &filepath)
{
    // TODO
    HZ_CORE_ASSERT(false);
    return false;
}

} // namespace hazel
