#include "hz_pch.h"

#include "glfw_window.h"


//
#include <GLFW/glfw3.h>
//

#include "platform/opengl/opengl_context.h"

#include "hazel/core/log.h"

#include "hazel/event/application_event.h"
#include "hazel/event/key_event.h"
#include "hazel/event/mouse_event.h"


namespace hazel {

static bool bGLFWInitialized = false;



Window *Window::Create(const WindowProps &props) { return new PlatGLFWWindow(props); }


PlatGLFWWindow::PlatGLFWWindow(const WindowProps &props)
{
    Init(props);
}
PlatGLFWWindow::~PlatGLFWWindow()
{
    ShutDown();
}


void PlatGLFWWindow::Init(const WindowProps &props)
{

    HZ_PROFILE_FUNCTION();

    m_Data.Title  = props.Title;
    m_Data.Width  = props.Width;
    m_Data.Height = props.Height;

    HZ_CORE_INFO("Creating Window \"{}\" [{}, {}]", props.Title, props.Width, props.Height);

    if (!bGLFWInitialized) {

        {
            HZ_PROFILE_SCOPE("Load GLAD");
            int bSuccess = glfwInit();
            HZ_CORE_ASSERT(GLFW_FALSE != bSuccess, "Could not initialize GLFW!");
            bGLFWInitialized = true;
        }
    }

    {
        HZ_PROFILE_SCOPE("Create/Init GLFW Context");
        m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, props.Title.c_str(), nullptr, nullptr);
        // m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, "hello", nullptr, nullptr);
        glfwMakeContextCurrent(m_Window);

        Window::m_Context = new hazel::OpenGLContext(m_Window);
        Window::m_Context->Init();

        glfwSetWindowUserPointer(m_Window, &m_Data);
    }


    //    DebugGLVerbose();
    initCallbacks();

    SetVSync(true);
}

void PlatGLFWWindow::ShutDown()
{
    HZ_PROFILE_FUNCTION();

    HZ_CORE_INFO("Deleting window context of opengl");
    glfwDestroyWindow(m_Window);
}

void PlatGLFWWindow::OnUpdate()
{
    HZ_PROFILE_FUNCTION();

    glfwPollEvents();
    m_Context->SwapBuffers();
}

void PlatGLFWWindow::SetVSync(bool bEnable)
{
    HZ_PROFILE_FUNCTION();

    glfwSwapInterval(bEnable ? 1 : 0);
    m_Data.bVSync = bEnable;
}

bool PlatGLFWWindow::IsVSync() const
{
    return m_Data.bVSync;
}

unsigned int PlatGLFWWindow::GetHeight() const { return m_Data.Height; }


void PlatGLFWWindow::initCallbacks()
{
    HZ_PROFILE_FUNCTION();

    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow *win, int w, int h) {
        if (WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(win))) {
            data->Width  = w;
            data->Height = h;
            WindowResizeEvent ev(w, h);
            HZ_CORE_INFO(ev.to_string());
            data->EventCallback(ev);
        }
    });

    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *win) {
        if (WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(win))) {
            WindowCloseEvent ev;
            data->EventCallback(ev);
        }
    });

    glfwSetKeyCallback(m_Window, [](GLFWwindow *win, int key, int scancode, int action, int mods) {
        if (WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(win))) {
            switch (action) {
                case GLFW_PRESS:
                {
                    KeyPressedEvent ev(key, 0);
                    data->EventCallback(ev);
                    break;
                }
                case GLFW_RELEASE:
                {
                    KeyReleasedEvent ev(key);
                    data->EventCallback(ev);
                    break;
                }
                case GLFW_REPEAT:
                {
                    KeyPressedEvent ev(key, 1);
                    data->EventCallback(ev);
                    break;
                }
            }
        }
    });

    glfwSetCharCallback(m_Window, [](GLFWwindow *win, unsigned int character) {
        if (WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(win))) {
            KeyTypedEvent ev(character);
            data->EventCallback(ev);
        }
    });

    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *win, int button, int action, int mods) {
        if (WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(win))) {
            switch (action) {

                case GLFW_PRESS:
                {
                    MouseButtonPressedEvent ev(button);
                    data->EventCallback(ev);
                    break;
                }
                case GLFW_RELEASE:
                {
                    MouseButtonReleasedEvent ev(button);
                    data->EventCallback(ev);
                    break;
                }
            }
        }
    });

    glfwSetScrollCallback(m_Window, [](GLFWwindow *win, double x_offset, double y_offset) {
        if (WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(win))) {
            MouseScrolledEvent ev((float)x_offset, (float)y_offset);
            data->EventCallback(ev);
        }
    });

    glfwSetCursorPosCallback(m_Window, [](GLFWwindow *win, double x, double y) {
        if (WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(win))) {
            MouseMoveEvent ev((float)x, (float)y);
            data->EventCallback(ev);
        }
    });
}



} // namespace hazel
