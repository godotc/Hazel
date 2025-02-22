//
/*
 * @ Author: godot42
 * @ Create Time: 2024-11-23 01:27:43
 * @ Modified by: @godot42
 * @ Modified time: 2024-12-28 05:03:53
 * @ Description:
 */


#include "editor_camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/fwd.hpp"
#include "hazel/core/base.h"
#include "hazel/core/input.h"
#include "hazel/core/key_code.h"
#include "hazel/core/mouse_button.h"
#include "hazel/event/event.h"
#include "hazel/event/key_event.h"
#include "hazel/event/mouse_event.h"
#include <cstddef>
#include <cstdint>
#include <glm/matrix.hpp>



#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


namespace hazel {

EditorCamera::EditorCamera()
{
}


EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
    : m_FOV(fov),
      m_AspectRatio(aspectRatio),
      m_NearClip(nearClip),
      m_FarClip(farClip),
      Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
{
    UpdateView();
}


void EditorCamera::OnUpdate(Timestep ts)
{
    static glm::vec2 m_InitialMousePos;
    if (Input::IsKeyPressed(Key::LeftAlt)) {
        const glm::vec2 &mouse{Input::GetMouseX(), Input::GetMouseY()};
        auto             delta = (mouse - m_InitialMousePos) * 0.003f;
        m_InitialMousePos      = mouse;

        if (Input::IsMouseButtonPressed(Mouse::Button_Middle)) {
            MousePan(delta);
        }
        else if (Input::IsMouseButtonPressed(Mouse::Button_Left)) {
            MouseRotate(delta);
        }
        else if (Input::IsMouseButtonPressed(Mouse::Button_Right)) {
            MouseZoom(delta.y);
        }
    }

    // static glm::vec2 m_LastPos;
    // if (Input::IsMouseButtonPressed(Mouse::Button_Right)) {
    //     const glm::vec2 &mouse{Input::GetMouseX(), Input::GetMouseY()};
    //     auto             delta = (mouse - m_InitialMousePos) * 0.003f;
    //     m_LastPos              = mouse;
    //     MouseRotate(delta);
    // }

    UpdateView();
}

void EditorCamera::UpdateProjection()
{
    m_AspectRatio = m_ViewportWidth / (float)m_ViewportHeight;
    m_Projection  = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
}

void EditorCamera::UpdateView()
{
    // m_Yaw = m_Pitch = 0.0f; // Lock the camera's rotation
    m_Position = CalculatePosition();

    glm::quat orientation = GetOrientation();
    m_ViewMatrix          = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
    m_ViewMatrix          = glm::inverse(m_ViewMatrix);
}

std::pair<float, float> EditorCamera::PanSpeed() const
{
    float x       = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
    float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

    float y       = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
    float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

    return {xFactor, yFactor};
}

float EditorCamera::RotationSpeed() const
{
    return 0.8f;
}

float EditorCamera::ZoomSpeed() const
{
    float distance = m_Distance * 0.2f;
    distance       = std::max(distance, 0.0f);
    float speed    = distance * distance;
    speed          = std::min(speed, 100.0f); // max speed = 100
    return speed;
}


void EditorCamera::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT(this, &EditorCamera::OnMouseScroll));
    dispatcher.Dispatch<KeyPressedEvent>(HZ_BIND_EVENT(this, &EditorCamera::OnKeyPressed));
}

bool EditorCamera::OnMouseScroll(MouseScrolledEvent &e)
{
    float delta = e.GetOffsetY() * 0.1f;
    MouseZoom(delta);
    UpdateView();
    return false;
}
bool EditorCamera::OnKeyPressed(KeyPressedEvent &e)
{
    //     switch (e.GetKeyCode()) {
    //         case Key::W:
    //         {
    //             auto dir = GetForwardDirection();
    //             m_Position += dir * 50.f;
    //             break;
    //         }
    //     }
    return false;
}

void EditorCamera::MousePan(const glm::vec2 &delta)
{
    auto [xSpeed, ySpeed] = PanSpeed();
    m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
    m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
}

void EditorCamera::MouseZoom(float dt)
{
    m_Distance -= dt * ZoomSpeed();
    if (m_Distance < 1.0f)
    {
        m_FocalPoint += GetForwardDirection();
        m_Distance = 1.0f;
    }
}


void EditorCamera::MouseRotate(const glm::vec2 &delta)
{
    float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
    m_Yaw += yawSign * delta.x * RotationSpeed();
    m_Pitch += delta.y * RotationSpeed();
}


glm::vec3 EditorCamera::GetUpDirection() const
{
    return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 EditorCamera::GetRightDirection() const
{
    return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::vec3 EditorCamera::GetForwardDirection() const
{
    return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::vec3 EditorCamera::CalculatePosition() const
{
    return m_FocalPoint - GetForwardDirection() * m_Distance;
}

glm::quat EditorCamera::GetOrientation() const
{
    return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
}

} // namespace hazel
