//
// Created by nono on 9/22/23.
//

#ifndef HAZEL_ORTHOGRAPHIC_CAMERA_H
#define HAZEL_ORTHOGRAPHIC_CAMERA_H

#include <glm/glm.hpp>

namespace hazel {

class OrthographicsCamera
{
  public:
    OrthographicsCamera(float left, float right, float bottom, float top);
    void SetProjection(float left, float right, float bottom, float top);

  public:
    [[nodiscard]] const glm::vec3 &GetPosition() const;
    void                           SetPosition(const glm::vec3 &mPosition);

    [[nodiscard]] float GetRotation() const;
    void                SetRotation(float mRotation);

    [[nodiscard]] const glm::mat4 &GetProjectionMatrix() const;
    void                           SetProjectionMatrix(const glm::mat4 &mProjectionMatrix);

    [[nodiscard]] const glm::mat4 &GetViewMatrix() const;
    void                           SetViewMatrix(const glm::mat4 &mViewMatrix);

    [[nodiscard]] const glm::mat4 &GetViewProjectionMatrix() const;
    void                           SetViewProjectionMatrix(const glm::mat4 &mViewProjectionMatrix);

  private:
    void RecalculateViewMatrix();

  private:
    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ViewProjectionMatrix;

  public:
  private:
    glm::vec3 m_Position = {0.f, 0.f, 0.f};
    float     m_Rotation = {0.f};
};



} // namespace hazel

#endif // HAZEL_ORTHOGRAPHIC_CAMERA_H
