//
// Created by nono on 9/23/23.
//
#include "hz_pch.h"

#include "gl_macros.h"
#include "hazel/core.h"

#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include "opengl_shader.h"

namespace hazel {

OpenGLShader::OpenGLShader(const std::string &vert_src, const std::string &frag_src, const std::string &geom_src)
{
    unsigned int vertex_shader, fragment_shader, geometry_shader;

    auto vertex_source   = static_cast<const GLchar *>(vert_src.c_str());
    auto fragment_source = static_cast<const GLchar *>(frag_src.c_str());
    auto geometry_source = geom_src.empty() ? nullptr : static_cast<const GLchar *>(geom_src.c_str());

    // vertex Shader
    GL_CALL(vertex_shader = glCreateShader(GL_VERTEX_SHADER));
    GL_CALL(glShaderSource(vertex_shader, 1, &vertex_source, nullptr));
    GL_CALL(glCompileShader(vertex_shader));
    checkCompileErrors(vertex_shader, "VERTEX");


    // fragment Shader
    GL_CALL(fragment_shader = glCreateShader(GL_FRAGMENT_SHADER));
    GL_CALL(glShaderSource(fragment_shader, 1, &fragment_source, nullptr));
    GL_CALL(glCompileShader(fragment_shader));
    checkCompileErrors(fragment_shader, "FRAGMENT");

    // if geometry shader source code is given, also compile geometry shader
    if (geometry_source != nullptr)
    {
        GL_CALL(geometry_shader = glCreateShader(GL_GEOMETRY_SHADER));
        GL_CALL(glShaderSource(geometry_shader, 1, &geometry_source, nullptr));
        GL_CALL(glCompileShader(geometry_shader));
        checkCompileErrors(geometry_shader, "GEOMETRY");
    }

    // shader program
    this->m_ShaderID = glCreateProgram();
    GL_CALL(glAttachShader(this->m_ShaderID, vertex_shader));
    GL_CALL(glAttachShader(this->m_ShaderID, fragment_shader));
    if (geometry_source != nullptr) {
        GL_CALL(glAttachShader(this->m_ShaderID, geometry_shader));
    }

    GL_CALL(glLinkProgram(this->m_ShaderID));
    checkCompileErrors(this->m_ShaderID, "PROGRAM");

    // delete the shaders as they're linked into our program now and no longer necessary
    GL_CALL(glDeleteShader(vertex_shader));
    GL_CALL(glDeleteShader(fragment_shader));
    if (geometry_source != nullptr) {
        GL_CALL(glDeleteShader(geometry_shader));
    }
}

void OpenGLShader::checkCompileErrors(unsigned int object, const std::string &type)
{
    int  success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        GL_CALL(glGetShaderiv(object, GL_COMPILE_STATUS, &success));
        if (!success)
        {
            GL_CALL(glGetShaderInfoLog(object, 1024, nullptr, infoLog));
            HZ_CORE_ERROR("| ERROR::SHADER: Compile-time error: Type: {}\n"
                          "{} \n -- --------------------------------------------------- -- ",
                          type, infoLog);
        }
    }
    else
    {
        GL_CALL(glGetProgramiv(object, GL_LINK_STATUS, &success));
        if (!success)
        {
            GL_CALL(glGetProgramInfoLog(object, 1024, nullptr, infoLog));
            HZ_CORE_ERROR("| ERROR::Shader: Link-time error: Type: {}\n"
                          "{} \n -- --------------------------------------------------- -- ",
                          type, infoLog);
        }
    }
}

void OpenGLShader::Bind() const
{
    glUseProgram(m_ShaderID);
}
void OpenGLShader::Unbind() const
{
    glUseProgram(0);
}
void OpenGLShader::UploadUniformMat4(const std::string name, const glm::mat4 &matrix)
{
    // transpose: A^t
    glUniformMatrix4fv(glGetUniformLocation(m_ShaderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}
void OpenGLShader::UploadUniformFloat4(const std::string name, const glm::vec4 &float4)
{
    //    glUniform4fv(glGetUniformLocation(m_ShaderID, name.c_str()), 1, glm::value_ptr(float4));
    glUniform4f(glGetUniformLocation(m_ShaderID, name.c_str()), float4.x, float4.y, float4.z, float4.z);
}
void OpenGLShader::UploadUniformFloat(const std::string name, const float value)
{
    glUniform1f(glGetUniformLocation(m_ShaderID, name.c_str()), value);
}
void OpenGLShader::UploadUniformFloat2(const std::string name, const glm::vec2 &values)
{
    glUniform2f(glGetUniformLocation(m_ShaderID, name.c_str()), values.x, values.y);
}
void OpenGLShader::UploadUniformFloat3(const std::string name, const glm::vec3 &values)
{
    GL_CALL(glUniform3f(glGetUniformLocation(m_ShaderID, name.c_str()), values.x, values.y, values.z));
}
void OpenGLShader::UploadUniformInt(const std::string name, const int32_t value)
{
    // GL_CALL(
    glUniform1i(glGetUniformLocation(m_ShaderID, name.c_str()), value) /*)*/;
}
} // namespace hazel