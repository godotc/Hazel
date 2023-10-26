//
// Created by nono on 9/16/23.
//
#include "hz_pch.h"

#include "linux_input.h"

#include "GLFW/glfw3.h"
#include "hazel/core/app.h"

namespace hazel {

Input *Input::m_Instance = new LinuxInput();

bool LinuxInput::IsKeyPressedImpl(int keycode)
{
    auto *Window = any_cast<GLFWwindow *>(App::Get().GetWindow().GetNativeWindow());
    auto  state  = glfwGetKey(Window, keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool LinuxInput::IsMouseButtonPressedImpl(int button)
{
    auto *Window = any_cast<GLFWwindow *>(App::Get().GetWindow().GetNativeWindow());
    auto  state  = glfwGetMouseButton(Window, button);
    return state == GLFW_PRESS;
}

std::pair<float, float> LinuxInput::GetMousePosImpl()
{
    auto  *Window = any_cast<GLFWwindow *>(App::Get().GetWindow().GetNativeWindow());
    double x, y;
    glfwGetCursorPos(Window, &x, &y);
    return {(float)x, (float)y};
}

float LinuxInput::GetMouseXImpl()
{
    return GetMousePos().first;
}
float LinuxInput::GetMouseYImpl()
{
    return GetMousePos().second;
}


} // namespace hazel