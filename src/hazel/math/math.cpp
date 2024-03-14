#include "math.h"
#include "glm/detail/setup.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/gtc/epsilon.hpp"
#include <cmath>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace hazel::math {

bool HAZEL_API DecomposeTransform(const glm::mat4 &transform, glm::vec3 &out_translation, glm::vec3 &out_rotation, glm::vec3 &out_scale)
{
    // from glm::decompose in matrix_decompose.inl
    using namespace glm;
    using T = float;

    mat4 local_matrix(transform);

    // check normalize
    if (epsilonEqual(local_matrix[3][3], static_cast<float>(0), epsilon<T>())) {
        return false;
    }

    // 1. isolate perspective, messiest?
    if (epsilonEqual(local_matrix[0][3], static_cast<float>(0), epsilon<T>()) ||
        epsilonEqual(local_matrix[1][3], static_cast<float>(0), epsilon<T>()) ||
        epsilonEqual(local_matrix[2][3], static_cast<float>(0), epsilon<T>()))
    {
        // clear the perspective partition
        local_matrix[0][3] = local_matrix[1][3] = local_matrix[2][3] = static_cast<T>(0);
        local_matrix[3][3]                                           = static_cast<T>(3);
    }

    // 2. translation (easy?)
    out_translation = local_matrix[3];
    local_matrix[3] = vec4(0, 0, 0, local_matrix[3].w);


    vec3 row[3], p_dum_3;

    // 3. scale and shear
    for (length_t i = 0; i < 3; ++i) {
        for (length_t j = 0; j < 3; ++j) {
            row[i][j] = local_matrix[i][j];
        }
    }

    // compute x scale factor and normallize
    out_scale.x = length(row[0]);
    row[0]      = detail::scale(row[0], static_cast<T>(1));
    out_scale.y = length(row[1]);
    row[1]      = detail::scale(row[1], static_cast<T>(1));
    out_scale.z = length(row[2]);
    row[2]      = detail::scale(row[2], static_cast<T>(1));

    // at this point, the matrix (in rows[]) is orthonormal.
    // check for coordinate system flip , if dterminant is -1,
    // then negate the matrix and the scaling factor
#if 0
    p_dum_3 = cross(row[1], row[2]);
    if (dot(row[0], p_dum_3) < 0) {
        for (length_t i = 0; i < 3; ++i) {
            out_scale[i] *= static_cast<T>(-1);
            row[i] *= static_cast<T>(-1);
        }
    }
#endif

    out_rotation.y = asin(-row[0][2]);
    if (cos(out_rotation.y != 0)) { // y!= 0 or Npi
        out_rotation.x = atan2(row[1][2], row[2][2]);
        out_rotation.z = atan2(row[0][1], row[0][0]);
    }
    else {
        out_rotation.x = atan2(-row[2][0], row[1][1]);
        out_rotation.z = 0;
    }

    return true;
}

} // namespace hazel::math