#include "glm/ext/vector_float3.hpp"
#include "hazel/scene/scene_camera.h"
#include "hz_pch.h"

#include "component.h"
#include "hazel/core/log.h"
#include "scene.h"


#include "hazel/core/base.h"
#include "hazel/scene/entity.h"
#include "scene_serializer.h"
#include "yaml-cpp/emitter.h"
#include "yaml-cpp/emittermanip.h"
#include "yaml-cpp/node/node.h"
#include "yaml-cpp/node/parse.h"
#include "yaml-cpp/traits.h"



#include <cstdint>
#include <fstream>
#include <sstream>
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

} // namespace YAML

namespace hazel {



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

SceneSerializer::SceneSerializer(const Ref<Scene> &scene)
{
    m_Scene = scene;
}


static void SerializeEntity(YAML::Emitter &out, Entity &entity)
{
    out << YAML::BeginMap;
    out << YAML::Key << "Entity" << YAML::Value << "TODO";

    if (entity.HasComponent<TagComponent>()) {
        auto &comp = entity.GetComponent<TagComponent>();
        out << YAML::Key << "TagComponent";
        out << YAML::BeginMap;
        {
            out << YAML::Key << "Tag" << YAML::Value << comp.Tag;
        }
        out << YAML::EndMap;
    }
    if (entity.HasComponent<TransformComponent>()) {
        auto &comp = entity.GetComponent<TransformComponent>();
        out << YAML::Key << "TranformComponent";
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
        auto comp = entity.GetComponent<SpriteRendererComponent>();


        out << YAML::Key << "SpriteRendererComponent";

        out << YAML::BeginMap;
        {
            // get the color as a key and the value to serilize
            out << YAML::Key << "Color" << YAML::Value << comp.Color;
        }
        out << YAML::EndMap;
    }
    if (entity.HasComponent<NativeScriptComponent>()) {
        auto comp = entity.GetComponent<NativeScriptComponent>();

        out << YAML::Key << "NativeScriptComponent";

        out << YAML::BeginMap;
        {
            // out << YAML::Key << "Color" << YAML::Value << comp.;
        }
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
            m_Scene->m_Registry.each([&](auto entity_id) {
                Entity entity{entity_id, m_Scene.get()};
                if (!entity) {
                    HZ_WARN("An entity is invalid!");
                    return;
                }
                SerializeEntity(out, entity);
            });
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
    std::ifstream     f(filepath);
    std::stringstream strstream;
    strstream << f.rdbuf();

    YAML::Node data = YAML::Load(strstream.str());
    if (!data["Scene"]) {
        return false;
    }

    std::string scene_name = data["Scene"].as<std::string>();
    HZ_CORE_TRACE("Deserializing scene '{}'", scene_name);

    auto entites = data["Entities"];
    if (entites) {
        for (auto entity : entites) {
            // TODO
            // uint64_t uuid = entity["Entity"].as<uint64_t>();
            std::string uuid = entity["Entity"].as<std::string>();

            std::string name;
            auto        tag_component = entity["TagComponent"];
            if (tag_component) {
                name = tag_component["Tag"].as<std::string>();
            }

            HZ_CORE_TRACE("Deserialized entity with ID = {}, name = {}", uuid, name);
            Entity deserilized_entity = m_Scene->CreateEntity(name);

            auto transform_component = entity["TransformComponent"];
            if (transform_component) {
                auto &tc       = deserilized_entity.GetOrAddComponent<TransformComponent>();
                tc.Translation = transform_component["Translation"].as<glm::vec3>();
                tc.Rotation    = transform_component["Rotation"].as<glm::vec3>();
                tc.Scale       = transform_component["Scale"].as<glm::vec3>();
            }

            const auto &camera_component = entites["CameraComponent"];
            if (camera_component) {
                auto &cc = deserilized_entity.AddComponent<CameraComponent>();

                cc.bPrimary          = camera_component["bPrimary"].as<bool>();
                cc.bFixedAspectRatio = camera_component["bFixAspectRatio"].as<bool>();

                const auto camera_props = camera_component["Camera"];

                cc.Camera.SetProjectionType(static_cast<SceneCamera::ProjectionType>(camera_props["ProjectionType"].as<int>()));

                cc.Camera.SetOrthographicNear(camera_props["OrthographicNear"].as<float>());
                cc.Camera.SetOrthographicFar(camera_props["OrthographicFar"].as<float>());
                cc.Camera.SetOrthographicSize(camera_props["OrthographicSize"].as<float>());

                cc.Camera.SetPerspectiveFar(camera_props["PerspectiveFar"].as<float>());
                cc.Camera.SetPerspectiveNear(camera_props["PerspectiveNear"].as<float>());
                cc.Camera.SetPerspectiveFovy(camera_props["PerspectiveFovy"].as<float>());
            }

            const auto &spriterenderer_component = entites["SpriteRendererComponent"];
            if (spriterenderer_component) {
                auto &tc = deserilized_entity.AddComponent<SpriteRendererComponent>();
                tc.Color = spriterenderer_component["Color"].as<glm::vec4>();
            }
        }
    }



    return false;
}

bool SceneSerializer::DeserializeRuntime(const std::string &filepath)
{
    // TODO
    HZ_CORE_ASSERT(false);
    return false;
}

} // namespace hazel
