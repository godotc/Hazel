/**
 *  Author: @godot42
 *  Create Time: 2024-03-27 01:11:53
 *  Modified by: @godot42
 *  Modified time: 2024-03-27 02:11:46
 *  Description:
 */



#include "fwd.h"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>


namespace utils {

#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31



static shaderc_shader_kind GLShaderStageToShaderC(unsigned int Stage)
{
    switch (Stage) {
        case GL_VERTEX_SHADER:
            return shaderc_glsl_vertex_shader;
        case GL_FRAGMENT_SHADER:
            return shaderc_glsl_fragment_shader;
    }
    throw "Unknown shader type!";
    return shaderc_shader_kind(0);
}
} // namespace utils

namespace vkwrapper {


struct OpenGlCompilerPimpl {
    shaderc::Compiler       compiler;
    shaderc::CompileOptions options;
};

struct VulkanCompilerPImpl {
    shaderc::Compiler       compiler;
    shaderc::CompileOptions options;
};

VulkanCompiler::VulkanCompiler()
{
    impl = new VulkanCompilerPImpl;
    impl->options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
    const bool optimize = true;
    if (optimize) {
        impl->options.SetOptimizationLevel(shaderc_optimization_level_performance);
    }
}

void VulkanCompiler::Compile(const std::string &source,
                             const char        *input_file_name,
                             unsigned int       opengl_stage,
                             /*out*/
                             std::vector<uint32_t> &out_binary)
{
    shaderc::SpvCompilationResult result = impl->compiler.CompileGlslToSpv(
        source,
        utils::GLShaderStageToShaderC(opengl_stage),
        input_file_name,
        impl->options);

    if (result.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        throw result.GetErrorMessage();
        assert(false);
    }

    out_binary = std::vector<uint32_t>{result.begin(), result.end()};
}

OpenGlCompiler::OpenGlCompiler()
{
    if (optimize) {
        impl->options.SetOptimizationLevel(shaderc_optimization_level_performance);
    }

    impl->options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
}

void OpenGlCompiler::Compile(const std::vector<uint32_t> &spirv_binary,
                             const char                  *input_file_name,
                             unsigned int                 opengl_stage,

                             /*out*/
                             std::string           &gl_source,
                             std::vector<uint32_t> &out_binary)
{

    spirv_cross::CompilerGLSL glsl_compiler(spirv_binary);
    glsl_compiler.compile();
    gl_source = glsl_compiler.compile();

    shaderc::SpvCompilationResult result = impl->compiler.CompileGlslToSpv(
        std::add_const_t<decltype(gl_source)>(gl_source),
        utils::GLShaderStageToShaderC(opengl_stage),
        input_file_name);

    if (result.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        throw result.GetErrorMessage();
        assert(false);
    }
    out_binary = std::vector<uint32_t>{result.begin(), result.end()};
}

ReflectOutput vkwrapper::Reflect(const std::vector<uint32_t> &shader_data)
{
    ReflectOutput output;

    spirv_cross::Compiler        compiler(shader_data);
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    output.num_uniform_buffer = resources.uniform_buffers.size();
    output.num_sampled_image  = resources.sampled_images.size();

    for (const auto &resource : resources.uniform_buffers)
    {
        const auto &buffer_type  = compiler.get_type(resource.base_type_id);
        uint32_t    bufferSize   = compiler.get_declared_struct_size(buffer_type);
        uint32_t    binding      = compiler.get_decoration(resource.id, spv::DecorationBinding);
        std::size_t member_count = buffer_type.member_types.size();

        output.uniform_infos.push_back({resource.name,
                                        bufferSize,
                                        binding,
                                        member_count});
    }

    return output;
}

OpenGlCompiler::~OpenGlCompiler()
{
    delete impl;
}

VulkanCompiler::~VulkanCompiler()
{
    delete impl;
}


} // namespace vkwrapper