//
// Created by nono on 9/22/23.
//
#include "hz_pch.h"

#include "glm/gtc/matrix_transform.hpp"
#include "orthographic_camera.h"

namespace hazel {

const glm::mat4 &OrthographicCamera::GetProjectionMatrix() const
{
    return m_ProjectionMatrix;
}
void OrthographicCamera::SetProjectionMatrix(const glm::mat4 &mProjectionMatrix)
{
    m_ProjectionMatrix = mProjectionMatrix;
}
const glm::mat4 &OrthographicCamera::GetViewMatrix() const
{
    return m_ViewMatrix;
}
void OrthographicCamera::SetViewMatrix(const glm::mat4 &mViewMatrix)
{
    m_ViewMatrix = mViewMatrix;
}
const glm::mat4 &OrthographicCamera::GetViewProjectionMatrix() const
{
    return m_ViewProjectionMatrix;
}
void OrthographicCamera::SetViewProjectionMatrix(const glm::mat4 &mViewProjectionMatrix)
{
    m_ViewProjectionMatrix = mViewProjectionMatrix;
}
const glm::vec3 &OrthographicCamera::GetPosition() const
{
    return m_Position;
}
void OrthographicCamera::SetPosition(const glm::vec3 &mPosition)
{
    m_Position = mPosition;
    RecalculateViewMatrix();
}
float OrthographicCamera::GetRotation() const
{
    return m_Rotation;
}
void OrthographicCamera::SetRotation(float mRotation)
{
    m_Rotation = mRotation;
    RecalculateViewMatrix();
}

OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
    : m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.f, 1.f)),
      m_ViewMatrix(1.f)
{
    HZ_PROFILE_FUNCTION();
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
{
    HZ_PROFILE_FUNCTION();
    m_ProjectionMatrix     = glm::ortho(left, right, bottom, top, -1.f, 1.f);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}


void OrthographicCamera::RecalculateViewMatrix()
{
    HZ_PROFILE_FUNCTION();
    glm::mat4 transform =
        glm::translate(glm::mat4(1.f), m_Position) *
        glm::rotate(glm::mat4(1.f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

    m_ViewMatrix           = glm::inverse(transform);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

} // namespace hazel