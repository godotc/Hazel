#pragma once


#include "__microshit_api_hazel.h"
#include "hazel/renderer/camera.h"
#include <cstdint>

namespace hazel {

class HAZEL_API SceneCamera
    : public Camera
{
    float m_OrthographicSize = 10.f;
    float m_OrthographicNear = -1.f;
    float m_OrthographicFar  = 1.f;

    float m_AspectRatio = 0.f;

  public:
    SceneCamera();
    virtual ~SceneCamera() = default;

    void  SetOrthographic(float size, float near, float far);
    void  SetViewportSize(uint32_t w, uint32_t h);
    float GetOrthographicSize() const { return m_OrthographicSize; }
    void  SetOrthographicSize(float size);

  private:

    void RecalculateProjection();
};

} // namespace hazel