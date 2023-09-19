//
// Created by nono on 9/20/23.
//

#ifndef HAZEL_SHADER_H
#define HAZEL_SHADER_H

#include <cstdint>
#include <string>

namespace hazel {

class Shader
{
  public:
    Shader(const std::string &vert_src, const std::string &frag_src, const std::string &geom_src = "");
    ~Shader() {}

    void Bind() const;
    void Unbind() const;

  private:
    static void checkCompileErrors(unsigned int object, const std::string &type);

  private:
    uint32_t m_ShaderID;
};

} // namespace hazel

#endif // HAZEL_SHADER_H
