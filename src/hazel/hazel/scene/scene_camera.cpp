#include "scene_camera.h"
#include "glm/ext/matrix_clip_space.hpp"
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

    RecalculateProjection(); // Fixed function name
}

void SceneCamera::SetViewportSize(uint32_t w, uint32_t h)
{
    m_AspectRatio = w / (float)h;

    RecalculateProjection(); // Fixed function name
}

void SceneCamera::SetOrthographicSize(float size)
{
    m_OrthographicSize = size;
    RecalculateProjection();
}

void SceneCamera::RecalculateProjection() // Fixed function name
{
    float ortho_left   = -m_OrthographicSize * m_AspectRatio * 0.5f;
    float ortho_right  = m_OrthographicSize * m_AspectRatio * 0.5f;
    float ortho_bottom = -m_OrthographicSize * 0.5f;
    float ortho_top    = m_OrthographicSize * 0.5f;

    m_Projection = glm::ortho(ortho_left, ortho_right,
                              ortho_bottom, ortho_top,
                              m_OrthographicNear, m_OrthographicFar);
}

} // namespace hazel
