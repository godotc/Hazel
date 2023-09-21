//
// Created by nono on 9/18/23.
//
#include "hz_pch.h"

#include "opengl_context.h"

#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "hazel/core.h"
#include "hazel/log.h"



namespace hazel {

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

OpenGLContext::OpenGLContext(GLFWwindow *glfw_window_handle) : m_WindowHandle(glfw_window_handle)
{
    HZ_CORE_ASSERT(glfw_window_handle, "Window handle if null");
}
void OpenGLContext::Init()
{
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

    printGLVerbose();
}
void OpenGLContext::SwapBuffers()
{
    glfwSwapBuffers(m_WindowHandle);
}

void OpenGLContext::printGLVerbose()
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


} // namespace hazel
