//
// Created by nono on 9/22/23.
//

#include "orthographic_camera.h"
#include "glm/gtc/matrix_transform.hpp"

namespace hazel {

const glm::mat4 &OrthographicsCamera::GetProjectionMatrix() const
{
    return m_ProjectionMatrix;
}
void OrthographicsCamera::SetProjectionMatrix(const glm::mat4 &mProjectionMatrix)
{
    m_ProjectionMatrix = mProjectionMatrix;
}
const glm::mat4 &OrthographicsCamera::GetViewMatrix() const
{
    return m_ViewMatrix;
}
void OrthographicsCamera::SetViewMatrix(const glm::mat4 &mViewMatrix)
{
    m_ViewMatrix = mViewMatrix;
}
const glm::mat4 &OrthographicsCamera::GetViewProjectionMatrix() const
{
    return m_ViewProjectionMatrix;
}
void OrthographicsCamera::SetViewProjectionMatrix(const glm::mat4 &mViewProjectionMatrix)
{
    m_ViewProjectionMatrix = mViewProjectionMatrix;
}
const glm::vec3 &OrthographicsCamera::GetPosition() const
{
    return m_Position;
}
void OrthographicsCamera::SetPosition(const glm::vec3 &mPosition)
{
    m_Position = mPosition;
    RecalculateViewMatrix();
}
float OrthographicsCamera::GetRotation() const
{
    return m_Rotation;
}
void OrthographicsCamera::SetRotation(float mRotation)
{
    m_Rotation = mRotation;
    RecalculateViewMatrix();
}

OrthographicsCamera::OrthographicsCamera(float left, float right, float bottom, float top)
    : m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.f, 1.f)),
      m_ViewMatrix(1.f)
{
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void OrthographicsCamera::RecalculateViewMatrix()
{
    glm::mat4 transform =
        glm::translate(glm::mat4(1.f), m_Position) *
        glm::rotate(glm::mat4(1.f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

    m_ViewMatrix           = glm::inverse(transform);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

} // namespace hazel