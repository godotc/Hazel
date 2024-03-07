#pragma once

#include "__microshit_api_hazel.h"

#include "glm/fwd.hpp"

namespace hazel::math {

bool HAZEL_API DecomposeTansform(const glm::mat4 &transform,
                                 glm::vec3 &out_translation, glm::vec3 &out_rotation, glm::vec3 &out_scale);

}