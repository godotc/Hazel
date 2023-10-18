//
// Created by nono on 9/18/23.
//

#include "hz_pch.h"

#include "glad/glad.h"

#include "opengl_context.h"

#include "GLFW/glfw3.h"
#include "hazel/hazel/core/core.h"
#include "hazel/hazel/core/log.h"


namespace hazel {

static void MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                            const GLchar *message, const void *userParam);

OpenGLContext::OpenGLContext(GLFWwindow *glfw_window_handle)
{
    m_WindowHandle = glfw_window_handle;
    HZ_CORE_ASSERT(glfw_window_handle, "Window handle if null");
}
void OpenGLContext::Init()
{
    HZ_PROFILE_FUNCTION();

    int bSuccess = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
    HZ_CORE_ASSERT(bSuccess, "Could not initialize GLAD!");

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

    DebugGLVerbose();
}

void OpenGLContext::SwapBuffers()
{
    HZ_PROFILE_FUNCTION();
    glfwSwapBuffers(m_WindowHandle);
}

void OpenGLContext::DebugGLVerbose()
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

    HZ_CORE_ASSERT(major * 100 + minor * 10 > 450, "Required opengl version >= 4.5");
}

static void MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    // #define GL_DEBUG_SEVERITY_HIGH 0x9146
    // #define GL_DEBUG_SEVERITY_MEDIUM 0x9147
    // #define GL_DEBUG_SEVERITY_LOW 0x9148

    static int faltal_error = 0;

    if (severity < GL_DEBUG_SEVERITY_HIGH) {
        faltal_error = 0;
        HZ_CORE_WARN("{} type = 0x{:x} | severity = 0x{:x} | {}", "[GL]",
                     type, severity, message);
    }
    else {
        HZ_CORE_ERROR("{} type = 0x{:x} | severity = 0x{:x} | {}", "[GL]",
                      type, severity, message);
        if (++faltal_error > 10)
            PLATFORM_BREAK();
    }
}

} // namespace hazel
