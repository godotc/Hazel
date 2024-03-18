/**
 *  Author: @godot42
 *  Create Time: 2023-12-12 21:23:50
 *  Modified by: @godot42
 *  Modified time: 2024-03-19 02:03:39
 *  Description:
 */

#include "scene_camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "hazel/core/base.h"
#include <cstdint>

namespace hazel {

SceneCamera::SceneCamera()
{
}

void SceneCamera::SetOrthographic(float size, float the_near, float the_far)
{
    m_OrthographicSize = size;
    m_OrthographicNear = the_near;
    m_OrthographicFar  = the_far;

    RecalculateProjection();
}

void SceneCamera::SetViewportSize(uint32_t w, uint32_t h)
{
    HZ_CORE_ASSERT(w > 0 && h > 0);
    m_AspectRatio = w / (float)h;
    RecalculateProjection();
}

void SceneCamera::SetOrthographicSize(float size)
{
    m_OrthographicSize = size;
    RecalculateProjection();
}

void SceneCamera::SetOrthographicNear(float v)
{
    m_OrthographicNear = v;
    RecalculateProjection();
}

void SceneCamera::SetOrthographicFar(float v)
{
    m_OrthographicFar = v;
    RecalculateProjection();
}

void SceneCamera::SetProjectionType(ProjectionType type)
{
    m_ProjectionType = type;
    RecalculateProjection();
}

void SceneCamera::SetPerspectiveFovy(float PerspectiveFOV)
{
    m_PerspectiveFovy = PerspectiveFOV;
    RecalculateProjection();
}

void SceneCamera::SetPerspectiveNear(float PerspectiveNear)
{
    m_PerspectiveNear = PerspectiveNear;
    RecalculateProjection();
}

void SceneCamera::SetPerspectiveFar(float PerspectiveFar)
{
    m_PerspectiveFar = PerspectiveFar;
    RecalculateProjection();
}

void SceneCamera::RecalculateProjection() // Fixed function name
{
    switch (m_ProjectionType) {

        case ProjectionType::Perspective:
        {
            m_Projection = glm::perspective(m_PerspectiveFovy, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
            break;
        }
        case ProjectionType::Orthographic:
        {
            float ortho_left   = -m_OrthographicSize * m_AspectRatio * 0.5f;
            float ortho_right  = m_OrthographicSize * m_AspectRatio * 0.5f;
            float ortho_bottom = -m_OrthographicSize * 0.5f;
            float ortho_top    = m_OrthographicSize * 0.5f;

            m_Projection = glm::ortho(ortho_left, ortho_right,
                                      ortho_bottom, ortho_top,
                                      m_OrthographicNear, m_OrthographicFar);

            break;
        }
    }
}

} // namespace hazel
