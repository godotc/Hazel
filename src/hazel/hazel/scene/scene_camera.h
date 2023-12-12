#pragma once


#include "__microshit_api_hazel.h"
#include "glm/trigonometric.hpp"
#include "hazel/renderer/camera.h"
#include "hazel/scene/scene_camera.h"
#include <cstdint>

namespace hazel {

class HAZEL_API SceneCamera
    : public Camera
{
  public:
    enum class ProjectionType
    {
        Perspective  = 0,
        Orthographic = 1
    };

  private:

    float m_OrthographicSize = 10.f;
    float m_OrthographicNear = -1.f;
    float m_OrthographicFar  = 1.f;

    float m_PerspectiveFOV  = glm::radians(45.f);
    float m_PerspectiveNear = 0.01f;
    float m_PerspectiveFar  = 1000.f;

    float m_AspectRatio = 0.f;

  protected:
    ProjectionType m_ProjectionType = SceneCamera::ProjectionType::Perspective;

  public:

    SceneCamera();
    virtual ~SceneCamera() = default;


    void SetViewportSize(uint32_t w, uint32_t h);

    ProjectionType GetProjectionType() const { return m_ProjectionType; }
    void           SetProjectionType(ProjectionType type);

    // ~ Orthographic
    void SetOrthographic(float size, float near, float far);

    float GetOrthographicSize() const { return m_OrthographicSize; }
    void  SetOrthographicSize(float size);

    float GetOrthographicNearClip() const { return m_OrthographicNear; }
    void  SetOrthographicNearClip(float v);

    float GetOrthographicFarClip() const { return m_OrthographicFar; }
    void  SetOrthographicFarClip(float v);


    // ~ Perspective
    float GetPerspectiveFovy() const { return m_PerspectiveFOV; }
    void  SetPerspectiveFovy(float PerspectiveFovy);

    float GetPerspectiveNear() const { return m_PerspectiveNear; }
    void  SetPerspectiveNear(float PerspectiveNear);

    float GetPerspectiveFar() const { return m_PerspectiveFar; }
    void  SetPerspectiveFar(float PerspectiveFar);

  private:

    void RecalculateProjection();
};

} // namespace hazel