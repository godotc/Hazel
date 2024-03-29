#include "hz_pch.h"

#include "linux_window.h"


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



Window *Window::Create(const WindowProps &props) { return new LinuxWindow(props); }


LinuxWindow::LinuxWindow(const WindowProps &props)
{
    Init(props);
}
LinuxWindow::~LinuxWindow()
{
    ShutDown();
}


void LinuxWindow::Init(const WindowProps &props)
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
        HZ_PROFILE_SCOPE("Create/Init GLFW Contex");
        m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, props.Title.c_str(), nullptr, nullptr);
        // m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, "helo", nullptr, nullptr);
        glfwMakeContextCurrent(m_Window);

        Window::m_Context = new hazel::OpenGLContext(m_Window);
        Window::m_Context->Init();

        glfwSetWindowUserPointer(m_Window, &m_Data);
    }


    //    DebugGLVerbose();
    initCallbacks();

    SetVSync(true);
}

void LinuxWindow::ShutDown()
{
    HZ_PROFILE_FUNCTION();

    HZ_CORE_INFO("Deleting window context of opengl");
    glfwDestroyWindow(m_Window);
}

void LinuxWindow::OnUpdate()
{
    HZ_PROFILE_FUNCTION();

    glfwPollEvents();
    m_Context->SwapBuffers();
}

void LinuxWindow::SetVSync(bool bEnable)
{
    HZ_PROFILE_FUNCTION();

    glfwSwapInterval(bEnable ? 1 : 0);
    m_Data.bVSync = bEnable;
}

bool LinuxWindow::IsVSync() const
{
    return m_Data.bVSync;
}

unsigned int LinuxWindow::GetHeight() const { return m_Data.Height; }


void LinuxWindow::initCallbacks()
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

    glfwSetScrollCallback(m_Window, [](GLFWwindow *win, double xoffset, double yoffset) {
        if (WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(win))) {
            MouseScrolledEvent ev((float)xoffset, (float)yoffset);
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
