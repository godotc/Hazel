#pragma once


#include "glm/glm.hpp"

namespace hazel {


class Camera
{
  protected:
    glm::mat4 m_Projection;

  public:
    Camera() = default;
    Camera(const glm::mat4 &projection) : m_Projection(projection) {}
    virtual ~Camera() = default;

    const glm::mat4 &GetProjection() const { return m_Projection; }
};


} // namespace hazel