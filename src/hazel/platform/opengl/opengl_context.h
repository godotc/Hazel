//
// Created by nono on 9/18/23.
//
#pragma once

#include "hazel/renderer/graphics_context.h"

struct GLFWwindow;

namespace hazel {

class OpenGLContext : public GraphicsContext
{
  public:
    explicit OpenGLContext(GLFWwindow *glfw_window_handle);

    void Init() override;
    void SwapBuffers() override;

  private:
    void DebugGLVerbose();

  private:
    GLFWwindow *m_WindowHandle;
};

} // namespace hazel

