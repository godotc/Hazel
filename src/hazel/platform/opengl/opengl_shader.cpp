//
// Created by nono on 9/23/23.
//
#include "hz_pch.h"

#include "gl_macros.h"
#include "hazel/hazel/core/core.h"

#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include "opengl_shader.h"

namespace hazel {

static const char *eol_flag =
#if _WIN32
    "\r\n"
#elif __linux__
    "\n"
#endif
    ;

static GLenum ShaderTypeFromString(const std::string &type)
{
    if (type == "vertex")
        return GL_VERTEX_SHADER;
    else if (type == "fragment" || type == "pixel")
        return GL_FRAGMENT_SHADER;

    HZ_CORE_ASSERT(false, "Unknown shader type!");
    return 0;
}

OpenGLShader::OpenGLShader(const std::string &shader_file_path)
{
    m_ShaderID                 = 0;
    std::string source         = ReadFile(shader_file_path);
    auto        shader_sources = PreProcess(source);
    Compile(shader_sources);

    m_Name = std::filesystem::path(shader_file_path).stem().string();
    HZ_CORE_ASSERT(!m_Name.empty(), "Cannot derive the shader name from shader file.");
}

OpenGLShader::OpenGLShader(const std::string &name, const std::string &vert_src, const std::string &frag_src)
{
    m_ShaderID = 0;

    Compile(
        {
            {  GL_VERTEX_SHADER, vert_src},
            {GL_FRAGMENT_SHADER, frag_src}
    });
    m_Name = name;
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

std::string OpenGLShader::ReadFile(const std::string &shader_file_path)
{
    std::string   result;
    std::ifstream file(shader_file_path, std::ios::in | std::ios::binary);
    if (!file.is_open() || file.bad() || file.fail()) {
        auto msg = fmt::format("Failed to load shader from file: {}", shader_file_path);
        HZ_CORE_ASSERT(false, msg);
        HZ_CORE_ERROR(msg);
        return result;
    }
    if (!file) {
        auto msg = fmt::format("Failed to load shader from file: {}", shader_file_path);
        HZ_CORE_ASSERT(false, msg);
        HZ_CORE_ERROR(msg);
        return result;
    }

    file.seekg(0, std::ios::end);
    result.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(result.data(), result.size());
    file.close();

    return result;
}

std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string &source)
{
    std::unordered_map<GLenum, std::string> shader_sources;

    const char  *type_token     = "#type";
    const size_t type_token_len = strlen(type_token);
    size_t       pos            = source.find(type_token, 0);

    while (pos != std::string ::npos)
    {

        // get the type string
        size_t eol = source.find_first_of(eol_flag, pos);
        HZ_CORE_ASSERT(eol != std::string::npos, "Syntax error");
        size_t      begin = pos + type_token_len + 1;
        std::string type  = source.substr(begin, eol - begin);

        GLenum shader_type = ShaderTypeFromString(type);
        HZ_CORE_ASSERT(shader_type, "Invalid shader type specific");

        // get the shader content range
        size_t next_line_pos = source.find_first_not_of(eol_flag, eol);
        pos                  = source.find(type_token, next_line_pos);

        shader_sources[shader_type] =
            source.substr(next_line_pos,
                          pos - (next_line_pos == std::string::npos ? source.size() - 1 : next_line_pos));
    }

    return shader_sources;
}

void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string> &shader_sources)

{
    GLuint program;
    GL_CALL(program = glCreateProgram());
    HZ_CORE_ASSERT((shader_sources.size() <= 2), "Only 2 shaders for now");

    // TODO: use array for fixed numbers of shaders
    std::vector<GLuint> shaders;
    shaders.reserve(shader_sources.size());

    for (auto &[shader_type, source] : shader_sources)
    {
        GLuint shader_handle;
        // vertex Shader
        GL_CALL(shader_handle = glCreateShader(shader_type));
        const GLchar *src = source.c_str();
        GL_CALL(glShaderSource(shader_handle, 1, &src, nullptr));
        GL_CALL(glCompileShader(shader_handle));

        GLint success;
        GL_CALL(glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &success));
        if (!success) {
            GLint max_len = {0};
            GL_CALL(glGetShaderiv(shader_type, GL_INFO_LOG_LENGTH, &max_len));
            std::vector<GLchar> log(max_len);
            GL_CALL(glGetShaderInfoLog(shader_handle, max_len, &max_len, log.data()));

            glDeleteShader(shader_handle);

            HZ_CORE_ERROR("{}", log.data());
            HZ_CORE_ASSERT(false, "shader compilation failed!");

            break;
        }

        shaders.push_back(shader_handle);
    }

    for (auto shader : shaders) {
        //        GL_CALL(
        glAttachShader(program, shader);
        //);
    }
    GL_CALL(glLinkProgram(program));

    GLint bIsLinked;
    GL_CALL(glGetProgramiv(program, GL_LINK_STATUS, &bIsLinked));
    if (!bIsLinked) {
        GLint max_len = {0};
        GL_CALL(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_len));
        std::vector<GLchar> log(max_len);
        GL_CALL(glGetProgramInfoLog(program, max_len, &max_len, &log[0]));

        glDeleteProgram(program);
        for (auto shader : shaders) {
            GL_CALL(glDeleteShader(shader));
        }

        HZ_CORE_ERROR("Program Linkage: {0}", log.data());
        HZ_CORE_ASSERT(false, "shader compilation failed!");
        return;
    }

    // delete the shaders as they're linked into our program now and no longer necessary
    for (auto shader : shaders) {
        GL_CALL(glDeleteShader(shader));
    }

    m_ShaderID = program;
}


} // namespace hazel