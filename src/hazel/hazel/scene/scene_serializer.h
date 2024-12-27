//
/*
 * @ Author: godot42
 * @ Create Time: 2024-11-23 01:27:43
 * @ Modified by: @godot42
 * @ Modified time: 2024-12-28 03:59:34
 * @ Description:
 */

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