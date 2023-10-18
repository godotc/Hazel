//
// Created by nono on 9/20/23.
//

#ifndef HAZEL_SHADER_H
#define HAZEL_SHADER_H

#include <cstdint>
#include <string>

#include <glm/fwd.hpp>
#include <hazel/core/core.h>

namespace hazel {

class Shader
{
  public:
    static Ref<Shader> Create(const std::string &shader_file_path);
    static Ref<Shader> Create(const std::string &name, const std::string &vert_src, const std::string &frag_src);
    virtual ~Shader() = default;

    virtual void Bind() const   = 0;
    virtual void Unbind() const = 0;

    [[nodiscard]] virtual const std::string &GetName() const = 0;

  public:
    virtual void SetInt(const std::string &name, int32_t value)              = 0;
    virtual void SetFloat(const std::string &name, float value)              = 0;
    virtual void SetFloat3(const std::string &name, const glm::vec3 &values) = 0;
    virtual void SetFloat4(const std::string &name, const glm::vec4 &float4) = 0;
    virtual void SetMat4(const std::string &name, const glm::mat4 &matrix)   = 0;


  private:
    uint32_t m_ShaderID = 0;
};

class ShaderLibrary
{
  public:
    void        Add(const std::string &name, const Ref<Shader> &shader);
    void        Add(const Ref<Shader> &shader);
    Ref<Shader> Load(const std::string &file_path);
    Ref<Shader> Load(const std::string &name, const std::string &file_path);

    Ref<Shader> Get(const std::string &name);
    bool        Exist(const std::string &name) const;

  private:
    std::unordered_map<std::string, Ref<Shader>> m_Shaders{};
};

} // namespace hazel

#endif // HAZEL_SHADER_H
