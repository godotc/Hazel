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
    OpenGLShader(const std::string &name, const std::string &vert_src, const std::string &frag_src);
    ~OpenGLShader() override {}

    void Bind() const override;
    void Unbind() const override;

    [[nodiscard]] const std::string &GetName() const override { return m_Name; }

  public:
    void SetInt(const std::string &name, const int32_t value) override;
    void SetFloat(const std::string &name, const float value) override;
    void SetFloat3(const std::string &name, const glm::vec3 &values) override;
    void SetFloat4(const std::string &name, const glm::vec4 &float4) override;
    void SetMat4(const std::string &name, const glm::mat4 &matrix) override;


  public:
    void UploadUniformFloat(const std::string &name, const float value);
    void UploadUniformFloat2(const std::string &name, const glm::vec2 &values);
    void UploadUniformFloat3(const std::string &name, const glm::vec3 &values);
    void UploadUniformFloat4(const std::string &name, const glm::vec4 &float4);

    void UploadUniformInt(const std::string &name, const int32_t value);

    void UploadUniformMat4(const std::string &name, const glm::mat4 &matrix);

  private:
    std::string                             ReadFile(const std::string &file_path);
    std::unordered_map<GLenum, std::string> PreProcess(const std::string &source);
    void                                    Compile(const std::unordered_map<GLenum, std::string> &shader_sources);

  private:
    std::string m_Name{};
    uint32_t    m_ShaderID{0};
};

} // namespace hazel

#endif // HAZEL_OPEN_GL_SHADER_H
