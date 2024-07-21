//
// Created by nono on 10/10/23.
//

#ifndef HAZEL_ORTHOGRAPHIC_CAMERA_CONTROLLER_H
#define HAZEL_ORTHOGRAPHIC_CAMERA_CONTROLLER_H

#include "__microshit_api_hazel.h"

#include "hazel/core/timestep.h"
#include "hazel/renderer/orthographic_camera.h"

#include "hazel/event/application_event.h"
#include "hazel/event/event.h"
#include "hazel/event/mouse_event.h"

namespace hazel {



struct OrthographicCameraBounds {
    float Left, Right, Bottom, Top;
    float GetWidth() { return Right - Left; }
    float GetHeight() { return Top - Bottom; }
};

class HAZEL_API OrthographicCameraController
{
    float                     m_AspectRatio = 1.6f;
    float                     m_ZoomLevel   = 1.f;
    OrthographicCameraBounds m_Bounds;
    OrthographicCamera       m_Camera;

    bool bRotation;

    glm::vec3 m_CameraPosition         = {0.f, 0.f, 0.f};
    float     m_CameraRotation         = 0.f;
    float     m_CameraTranslationSpeed = 6.f;
    float     m_CameraRotationSpeed    = 90.f;

  public:
    OrthographicCameraController(float aspect_ratio, bool rotation);

    void OnUpdate(Timestep ts);
    void OnEvent(Event &ev);

    const OrthographicCamera &GetCamera() const { return m_Camera; }
    OrthographicCamera       &GetCamera() { return m_Camera; }

    void  SetZoomLevel(float level);
    float GetZoomLevel() { return m_ZoomLevel; }

    const OrthographicCameraBounds &GetBounds() { return m_Bounds; }
    void                             OnResize(float w, float h);

  private:
    bool OnMouseScrolled(MouseScrolledEvent &ev);
    bool OnWindowResized(WindowResizeEvent &ev);

    void RecalcualteView();
};

} // namespace hazel

#endif // HAZEL_ORTHOGRAPHIC_CAMERA_CONTROLLER_H
