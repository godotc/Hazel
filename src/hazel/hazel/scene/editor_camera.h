//
/*
 * @ Author: godot42
 * @ Modified by: @godot4223 01:27:43
 * @ Modified by: @godot42
 * @ Modified time: 2024-12-28 05:04:59
 * @ Description:
 */

#pragma once


#include "__microshit_api_hazel.h"

#include "hazel/core/timestep.h"
#include "hazel/event/key_event.h"
#include "hazel/event/mouse_event.h"
#include "hazel/renderer/camera.h"
#include <cstdint>

namespace hazel {

class HAZEL_API EditorCamera
    : public Camera
{
    glm::mat4 m_ViewMatrix;

    glm::vec3 m_Position   = {0.f, 0.f, 0.f};
    glm::vec3 m_FocalPoint = {0.f, 0.f, 0.f};

    float m_Distance = 5.f;

    float    m_AspectRatio = 1.6 / 0.9f;
    uint32_t m_ViewportWidth;
    uint32_t m_ViewportHeight;

    float m_FOV      = 45.f;
    float m_NearClip = -10.f, m_FarClip = 100.f;

  protected:
    float m_Pitch = 0.f,
          m_Yaw   = 0.f,
          m_Roll  = 0.f;

  public:
    EditorCamera();
    EditorCamera(float fov, float aspect_ratio, float near_clip, float far_clip);
    virtual ~EditorCamera() = default;

    void OnUpdate(Timestep ts);
    void OnEvent(Event &e);

  public:
    void SetViewportSize(uint32_t w, uint32_t h)
    {
        m_ViewportWidth = w, m_ViewportHeight = h;
        UpdateProjection();
    };

    glm::mat4        GetViewProjection() const { return m_Projection * m_ViewMatrix; }
    const glm::mat4 &GetViewMatrix() const { return m_ViewMatrix; }
    glm::vec3        GetPosition() const { return m_Position; }
    float            GetPitch() const { return m_Pitch; }
    float            GetYaw() const { return m_Yaw; }

    float GetDistance() const { return m_Distance; }
    void  SetDistance(float Distance) { m_Distance = Distance; }

    glm::vec3 GetUpDirection() const;
    glm::vec3 GetForwardDirection() const;
    glm::vec3 GetRightDirection() const;

    float GetNearClip() const { return m_NearClip; }
    void  SetNearClip(float v)
    {
        m_NearClip = v;
        UpdateProjection();
    }
    float GetFarClip() const { return m_FarClip; }
    void  setFarClip(float v)
    {
        m_NearClip = v;
        UpdateProjection();
    }
    float GetFOV() const { return m_FOV; }
    void  SetFOV(float v)
    {
        m_FOV = v;
        UpdateProjection();
    }

  private:

    void UpdateView();
    void UpdateProjection();
    void RecalculateProjection();

    void MouseRotate(const glm::vec2 &delta);
    void MousePan(const glm::vec2 &delta);
    void MouseZoom(float dt);

    std::pair<float, float> PanSpeed() const;
    float                   RotationSpeed() const;
    float                   ZoomSpeed() const;


    glm::vec3 CalculatePosition() const;
    glm::quat GetOrientation() const;

    bool OnMouseScroll(MouseScrolledEvent &e);
    bool OnKeyPressed(KeyPressedEvent &e);
};

} // namespace hazel