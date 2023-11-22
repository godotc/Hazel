#pragma once

#include "glm/detail/qualifier.hpp"
#include "glm/glm.hpp"
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

} // namespace hazel