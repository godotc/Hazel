#pragma once

#include "hazel/core/base.h"
#include "scene.h"
#include "utils/path.h"



namespace hazel {
class HAZEL_API SceneSerializer
{
    Ref<Scene> m_Scene;

  public:
    SceneSerializer(const Ref<Scene> &scene);

    void Serialize(const std::string &filepath);
    void SerializeRuntime(const std::string &filepath);

    bool Deserialize(const std::string &filepath);
    bool DeserializeRuntime(const std::string &filepath);
};


}; // namespace hazel