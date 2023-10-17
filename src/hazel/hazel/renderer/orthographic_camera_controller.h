//
// Created by nono on 10/10/23.
//

#ifndef HAZEL_ORTHOGRAPHIC_CAMERA_CONTROLLER_H
#define HAZEL_ORTHOGRAPHIC_CAMERA_CONTROLLER_H

#include "hazel/__microshit_api_hazel.h"

#include "hazel/core/timestep.h"
#include "hazel/renderer/orthographic_camera.h"

#include "hazel/event/application_event.h"
#include "hazel/event/event.h"
#include "hazel/event/mouse_event.h"

namespace hazel {

class OrthographicsCamera;

class HAZEL_API OrthographicsCameraController
{
  public:
    OrthographicsCameraController(float aspect_ratio, bool rotation);

    void OnUpdate(Timestep ts);
    void OnEvent(Event &ev);

    const OrthographicsCamera &GetCamera() const { return m_Camera; }
    OrthographicsCamera       &GetCamera() { return m_Camera; }

    void SetZoomLevel(float level);
    float GetZoomLevel() { return m_ZoomLevel; }


  private:
    bool OnMouseScrooled(MouseScrolledEvent &ev);
    bool OnWindowResized(WindowResizeEvent &ev);

    void ResetCmaeraProjection();


  private:
    float               m_AspectRatio = 1.6f;
    float               m_ZoomLevel   = 1.f;
    OrthographicsCamera m_Camera;

    bool bRotation;

    glm::vec3 m_CameraPosition         = {0.f, 0.f, 0.f};
    float     m_CameraRotation         = {0.f};
    float     m_CameraTranslationSpeed = {6.f},
          m_CameraRotationSpeed        = {90.f};
};

} // namespace hazel

#endif // HAZEL_ORTHOGRAPHIC_CAMERA_CONTROLLER_H
