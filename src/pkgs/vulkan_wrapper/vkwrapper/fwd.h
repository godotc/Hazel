/**
 *  Author: @godot42
 *  Create Time: 2024-03-27 01:11:43
 *  Modified by: @godot42
 *  Modified time: 2024-03-27 02:12:39
 *  Description:
 */



#include <cstdint>
#include <vector>

#include <string>


namespace vkwrapper {


struct VulkanCompilerPImpl;
class VulkanCompiler
{

    VulkanCompilerPImpl *impl;

  public:
    const bool optimize = true;

  public:
    VulkanCompiler();
    ~VulkanCompiler();

    void Compile(const std::string &source,
                 const char        *input_file_name,
                 unsigned int       opengl_stage,
                 /*out*/
                 std::vector<uint32_t> &out_binary);
};

struct OpenGlCompilerPimpl;
class OpenGlCompiler
{

    OpenGlCompilerPimpl *impl;


  public:
    const bool optimize = false;

  public:
    OpenGlCompiler();
    ~OpenGlCompiler();



    void Compile(const std::vector<uint32_t> &spirv_binary,
                 const char                  *input_file_name,
                 unsigned int                 opengl_stage,

                 /*out*/
                 std::string           &gl_source,
                 std::vector<uint32_t> &out_binary);
};


struct UniformReflectInfos {
    std::string name;
    std::size_t buffersize;
    uint32_t    binding;
    std::size_t member_count;
};

struct ReflectOutput {
    std::size_t                      num_uniform_buffer;
    std::size_t                      num_sampled_image;
    std::vector<UniformReflectInfos> uniform_infos;
};

extern ReflectOutput Reflect(const std::vector<uint32_t> &shader_data);

} // namespace vkwrapper