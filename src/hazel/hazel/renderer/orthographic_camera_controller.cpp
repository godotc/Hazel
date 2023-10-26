//
// Created by nono on 10/10/23.
//

#include "hz_pch.h"

#include "hazel/core/input.h"
#include "hazel/core/key_code.h"
#include "orthographic_camera_controller.h"

namespace hazel {

OrthographicsCameraController::OrthographicsCameraController(float aspect_ratio, bool rotation)
    : m_AspectRatio(aspect_ratio),
      m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel),
      bRotation(rotation)
{
}

void OrthographicsCameraController::OnUpdate(Timestep dt)
{
    HZ_PROFILE_FUNCTION();
        if (hazel::Input::IsKeyPressed(HZ_KEY_A)) m_CameraPosition.x -= m_CameraTranslationSpeed * dt;
        if (hazel::Input::IsKeyPressed(HZ_KEY_D)) m_CameraPosition.x += m_CameraTranslationSpeed * dt;
        if (hazel::Input::IsKeyPressed(HZ_KEY_W)) m_CameraPosition.y += m_CameraTranslationSpeed * dt;
        if (hazel::Input::IsKeyPressed(HZ_KEY_S)) m_CameraPosition.y -= m_CameraTranslationSpeed * dt;
    // for right forward within camera rotation
//    if (hazel::Input::IsKeyPressed(HZ_KEY_A)) {
//        m_CameraPosition.x -= std::cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * dt;
//        m_CameraPosition.y -= std::sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * dt;
//    }
//    if (hazel::Input::IsKeyPressed(HZ_KEY_D)) {
//        m_CameraPosition.x += std::cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * dt;
//        m_CameraPosition.y += std::sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * dt;
//    }
//    if (hazel::Input::IsKeyPressed(HZ_KEY_W)) {
//        m_CameraPosition.x += -std::cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * dt;
//        m_CameraPosition.y += std::sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * dt;
//    }
//    if (hazel::Input::IsKeyPressed(HZ_KEY_S)) {
//        m_CameraPosition.x -= -std::cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * dt;
//        m_CameraPosition.y -= std::sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * dt;
//    }
    m_Camera.SetPosition(m_CameraPosition);

    // clang-format off
    if(bRotation){
        if (hazel::Input::IsKeyPressed(HZ_KEY_Q)) m_CameraRotation -= m_CameraRotationSpeed * dt;
        if (hazel::Input::IsKeyPressed(HZ_KEY_E)) m_CameraRotation += m_CameraRotationSpeed * dt;
        m_Camera.SetRotation(m_CameraRotation);
    }
    // clang-format on

    // better experience in different zoom level
    m_CameraTranslationSpeed = m_ZoomLevel;
}
void OrthographicsCameraController::OnEvent(Event &ev)
{
    HZ_PROFILE_FUNCTION();

    EventDispatcher dispatcher(ev);
    dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT(this, &OrthographicsCameraController::OnMouseScrolled));
    dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT(this, &OrthographicsCameraController::OnWindowResized));
}
bool OrthographicsCameraController::OnMouseScrolled(MouseScrolledEvent &ev)
{
    HZ_PROFILE_FUNCTION();
    m_ZoomLevel -= ev.GetOffsetY();
    m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
    ResetCmaeraProjection();
    return false;
}
bool OrthographicsCameraController::OnWindowResized(WindowResizeEvent &ev)
{
    HZ_PROFILE_FUNCTION();

    // TODO: Optimizing the zoom operation
    m_AspectRatio = ev.GetWidth() / (float)ev.GetHeight();
    ResetCmaeraProjection();
    return false;
}
void OrthographicsCameraController::ResetCmaeraProjection()
{
    m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
}
void OrthographicsCameraController::SetZoomLevel(float level)
{
    m_ZoomLevel = level;
    ResetCmaeraProjection();
}



} // namespace hazel