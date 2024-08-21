//
// Created by nono on 9/18/23.
//

#include "hz_pch.h"

#include "glad/glad.h"

#include "opengl_context.h"

#include "GLFW/glfw3.h"
#include "hazel/core/base.h"
#include "hazel/core/log.h"
#include <sstream>


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
        // glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
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

    // FIXME: avoid noise, too many ouput info
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
        return;
    }

    std::stringstream ss;
    ss << '\n';

    ss << "---------------------opengl-callback-start------------" << '\n';
    ss << "source: " << source << '\n';
    ss << "message: " << message << '\n';
    ss << "type: ";
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            ss << "ERROR";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            ss << "DEPRECATED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            ss << "UNDEFINED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            ss << "PORTABILITY";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            ss << "PERFORMANCE";
            break;
        case GL_DEBUG_TYPE_OTHER:
            ss << "OTHER";
            break;
    }
    ss << '\n';

    ss << "id: " << id << '\n';
    ss << "severity: ";
    switch (severity) {
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            ss << "NOTIFICATION";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            ss << "LOW";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            ss << "MEDIUM";
            break;
        case GL_DEBUG_SEVERITY_HIGH:
            ss << "HIGH";
            break;
    }
    ss << '\n';
    ss << "---------------------opengl-callback-end--------------" << '\n';

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            HZ_CORE_ERROR(ss.str());
            break;
        default:
            HZ_CORE_INFO(ss.str());
    }
}

} // namespace hazel