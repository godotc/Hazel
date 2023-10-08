//
// Created by nono on 9/23/23.
//

#ifndef HAZEL_OPEN_GL_SHADER_H
#define HAZEL_OPEN_GL_SHADER_H

#include "hazel/renderer/shader.h"
#include <glad/glad.h>

namespace hazel {



class OpenGLShader : public Shader
{
  public:
    OpenGLShader(const std::string &shader_file_path);
    OpenGLShader(const std::string &vert_src, const std::string &frag_src);
    ~OpenGLShader() override {}

    void Bind() const override;
    void Unbind() const override;

  public:
    void UploadUniformFloat(const std::string name, const float value);
    void UploadUniformFloat2(const std::string name, const glm::vec2 &values);
    void UploadUniformFloat3(const std::string name, const glm::vec3 &values);
    void UploadUniformFloat4(const std::string name, const glm::vec4 &float4);

    void UploadUniformInt(const std::string name, const int32_t value);

    void UploadUniformMat4(const std::string name, const glm::mat4 &matrix);

  private:
    std::string                             ReadFile(const std::string &file_path);
    std::unordered_map<GLenum, std::string> PreProcess(const std::string &source);
    void                                    Compile(const std::unordered_map<GLenum, std::string> &shader_sources);

  private:
    uint32_t m_ShaderID{};
};

} // namespace hazel

#endif // HAZEL_OPEN_GL_SHADER_H
