//
// Created by nono on 9/23/23.
//

#ifndef HAZEL_OPEN_GL_SHADER_H
#define HAZEL_OPEN_GL_SHADER_H

#include "hazel/renderer/shader.h"

namespace hazel {

class OpenGLShader : public Shader

{
  public:
    OpenGLShader(const std::string &vert_src, const std::string &frag_src, const std::string &geom_src = "");
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
    static void checkCompileErrors(unsigned int object, const std::string &type);

  private:
    uint32_t m_ShaderID;
};

} // namespace hazel

#endif // HAZEL_OPEN_GL_SHADER_H
