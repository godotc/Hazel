//
// Created by nono on 9/16/23.
//

#include "hazel/core/input.h"

#include "GLFW/glfw3.h"
#include "hazel/core/app.h"

namespace hazel {

bool Input::IsKeyPressed(int keycode)
{
    auto *Window = any_cast<GLFWwindow *>(App::Get().GetWindow().GetNativeWindow());
    auto  state  = glfwGetKey(Window, keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::IsMouseButtonPressed(int button)
{
    auto *Window = any_cast<GLFWwindow *>(App::Get().GetWindow().GetNativeWindow());
    auto  state  = glfwGetMouseButton(Window, button);
    return state == GLFW_PRESS;
}

std::pair<float, float> Input::GetMousePos()
{
    auto  *Window = any_cast<GLFWwindow *>(App::Get().GetWindow().GetNativeWindow());
    double x, y;
    glfwGetCursorPos(Window, &x, &y);
    return {x, y};
}

float Input::GetMouseX()
{
    return GetMousePos().first;
}
float Input::GetMouseY()
{
    return GetMousePos().second;
}


} // namespace hazel