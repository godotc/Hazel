#include "event/application_event.h"
#include "event/event.h"
#include "event/key_event.h"
#include "event/mouse_event.h"
#include "hazel.h"

#include "hazel/window.h"
#include "linux_window.h"


#include "glad/glad.h"
//
#include <GLFW/glfw3.h>

#include "log.h"


#include <imgui_impl_glfw.h>
namespace hazel {

static bool bGLFWInitialized = false;
static bool bGLADInitialized = false;

static void MessageCallback(GLenum        source,
                            GLenum        type,
                            GLuint        id,
                            GLenum        severity,
                            GLsizei       length,
                            const GLchar *message,
                            const void   *userParam)
{
    if (type == GL_DEBUG_TYPE_ERROR) {
        HZ_CORE_WARN("{} type = 0x{:x} | severity = 0x{:x} \n\t{}", "[GL]",
                     type, severity, message);
    }
    else {
        HZ_CORE_ERROR("{} type = 0x{:x} | severity = 0x{:x} \n\t{}", "[GL]",
                      type, severity, message);
    }
}



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
    m_Data.Title  = props.Title;
    m_Data.Width  = props.Width;
    m_Data.Height = props.Height;

    HZ_CORE_INFO("Creating Window \"{}\" [{}, {}]", props.Title, props.Width, props.Height);

    if (!bGLFWInitialized) {
        int bSuccess = glfwInit();
        HZ_CORE_ASSERT(GLFW_FALSE != bSuccess, "Could not initialize GLFW!");
        bGLFWInitialized = true;
    }

    m_Window = glfwCreateWindow((int)props.Width, (int)props.Height,
                                props.Title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(m_Window);



    if (!bGLADInitialized) {
        int bSuccess = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
        HZ_CORE_ASSERT(bSuccess, "Could not initialize GLAD!");
        bGLADInitialized = true;

        // During init, enable debug output
        // Notice: this is a specific driver extension
        if (glDebugMessageCallback != nullptr) {
            glEnable(GL_DEBUG_OUTPUT);
            glDebugMessageCallback(MessageCallback, nullptr);
            HZ_CORE_INFO("Bound GL debug callback successfully");
        }
        else {
            HZ_CORE_WARN("glDebugMessageCallback is nullptr. Maybe your driver is not supportting this extionsion!");
        }
    }

    {
        const GLubyte *renderer    = glGetString(GL_RENDERER);
        const GLubyte *vendor      = glGetString(GL_VENDOR);
        const GLubyte *version     = glGetString(GL_VERSION);
        const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
        GLint          major, minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        HZ_CORE_INFO("GL Vendor : {}", (const char *)vendor);
        HZ_CORE_INFO("GL Renderer :{}", (const char *)renderer);
        HZ_CORE_INFO("GL Version (string) : {}", (const char *)version);
        HZ_CORE_INFO("GL Version (integer) : {}, {}", major, minor);
        HZ_CORE_INFO("GLSL Version : {}\n", (const char *)glslVersion);
    }


    SetVSync(true);

    glfwSetWindowUserPointer(m_Window, &m_Data);

    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow *win, int w, int h) {
        if (WindowData *data = static_cast<WindowData *>(glfwGetWindowUserPointer(win))) {
            data->Width  = w;
            data->Height = h;
            WindowResizeEvent ev(w, h);
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

void LinuxWindow::ShutDown()
{
    glfwDestroyWindow(m_Window);
}

void LinuxWindow::OnUpdate()
{
    glfwPollEvents();
    glfwSwapBuffers(m_Window);
}

void LinuxWindow::SetVSync(bool bEnable)
{
    glfwSwapInterval(bEnable ? 1 : 0);
    m_Data.bVSync = bEnable;
}

bool LinuxWindow::IsVSync() const
{
    return m_Data.bVSync;
}
unsigned int LinuxWindow::GetHeight() const { return m_Data.Height; }

} // namespace hazel