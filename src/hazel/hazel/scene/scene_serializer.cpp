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



#include <fstream>
#include <sstream>
#include <yaml-cpp/yaml.h>

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

template <class T>
static void SerializeComponent(YAML::Emitter &out, T &entity)
{
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

            out << YAML::Key << "ProjectionType" << YAML::Value << (int)comp.Camera.GetProjectionType();

            out << YAML::Key << "PerspectiveFovy" << YAML::Value << comp.Camera.GetPerspectiveFovy();
            out << YAML::Key << "PerspectiveFar" << YAML::Value << comp.Camera.GetPerspectiveFar();
            out << YAML::Key << "PerspectiveNear" << YAML::Value << comp.Camera.GetPerspectiveNear();

            out << YAML::Key << "OrthographicSize" << YAML::Value << comp.Camera.GetOrthographicSize();
            out << YAML::Key << "OrthographicNearClip" << YAML::Value << comp.Camera.GetOrthographicNearClip();
            out << YAML::Key << "OrthographicFarClip" << YAML::Value << comp.Camera.GetOrthographicFarClip();
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
    out << YAML::Key << "Scene" << YAML::Value << "TODO";
    out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
    m_Scene->m_Registry.each([&](auto entity_id) {
        Entity entity{entity_id, m_Scene.get()};
        if (!entity) {
            HZ_WARN("An entity is invalid!");
            return;
        }
        SerializeEntity(out, entity);
    });
    out << YAML::EndSeq;
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

    return false;
}

bool SceneSerializer::DeserializeRuntime(const std::string &filepath)
{
    // TODO
    HZ_CORE_ASSERT(false);
    return false;
}

} // namespace hazel
