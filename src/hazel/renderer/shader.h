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
    static Shader *Create(const std::string &shader_file_path);
    static Shader *Create(const std::string &vert_src, const std::string &frag_src);
    virtual ~Shader() = default;

    virtual void Bind() const   = 0;
    virtual void Unbind() const = 0;

  private:
    uint32_t m_ShaderID;
};

} // namespace hazel

#endif // HAZEL_SHADER_H
