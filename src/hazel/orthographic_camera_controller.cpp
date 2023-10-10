//
// Created by nono on 10/10/23.
//

#include "hz_pch.h"

#include "hazel/input.h"
#include "hazel/key_code.h"
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
    // clang-format off
    if (hazel::Input::IsKeyPressed(HZ_KEY_A)) m_CameraPosition.x -= m_CameraTranslationSpeed * dt;
    if (hazel::Input::IsKeyPressed(HZ_KEY_D)) m_CameraPosition.x += m_CameraTranslationSpeed * dt;
    if (hazel::Input::IsKeyPressed(HZ_KEY_W)) m_CameraPosition.y += m_CameraTranslationSpeed * dt;
    if (hazel::Input::IsKeyPressed(HZ_KEY_S)) m_CameraPosition.y -= m_CameraTranslationSpeed * dt;
    m_Camera.SetPosition(m_CameraPosition);

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
    EventDispatcher dispatcher(ev);
    dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT(this, &OrthographicsCameraController::OnMouseScrooled));
    dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT(this, &OrthographicsCameraController::OnWindowResized));
}
bool OrthographicsCameraController::OnMouseScrooled(MouseScrolledEvent &ev)
{
    m_ZoomLevel -= ev.GetOffsetY();
    m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
    ResetCmaeraProjection();
    return false;
}
bool OrthographicsCameraController::OnWindowResized(WindowResizeEvent &ev)
{
    // TODO: Optimizing the zoom operation
    m_AspectRatio = ev.GetWidth() / (float)ev.GetHeight();
    ResetCmaeraProjection();
    return false;
}
void OrthographicsCameraController::ResetCmaeraProjection()
{
    m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
}



} // namespace hazel