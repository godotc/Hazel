/**
 *  Author: @godot42
 *  Create Time: 2023-11-17 23:45:29
 *  Modified by: @godot42
 *  Modified time: 2024-07-28 19:16:08
 *  Description:
 */

//
// Created by nono on 9/23/23.
//


#include "hz_pch.h"

#include "hazel/core/timer.h"

#include "hazel/core/log.h"
#include "hazel/debug/instrumentor.h"

#include "gl_macros.h"
#include "hazel/core/base.h"

#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include "nlohmann/json_fwd.hpp"
#include "opengl_shader.h"
#include "utils/path.h"



#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <ios>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "nlohmann/json.hpp"
#include "utils/file.h"



namespace hazel {

static const char *eol_flag =
#if _WIN32
    "\r\n";
#elif __linux__
    "\n";
#endif

namespace utils {

static GLenum ShaderTypeFromString(const std::string &type)
{
    if (type == "vertex")
        return GL_VERTEX_SHADER;
    else if (type == "fragment" || type == "pixel")
        return GL_FRAGMENT_SHADER;

    HZ_CORE_ASSERT(false, "Unknown shader type!");
    return 0;
}

static shaderc_shader_kind GLShaderStageToShaderRcType(GLenum Stage)
{
    switch (Stage) {
        case GL_VERTEX_SHADER:
            return shaderc_glsl_vertex_shader;
        case GL_FRAGMENT_SHADER:
            return shaderc_glsl_fragment_shader;
    }
    HZ_CORE_ASSERT(false, "Unknown shader type!");
    return shaderc_shader_kind(0);
}

static const char *GLShaderStageToString(GLenum stage)
{
    switch (stage)
    {
        case GL_VERTEX_SHADER:
            return "GL_VERTEX_SHADER";
        case GL_FRAGMENT_SHADER:
            return "GL_FRAGMENT_SHADER";
    }
    HZ_CORE_ASSERT(false);
    return nullptr;
}

static std::string           cache_relative_path = "res/cache/shader/opengl";
static std::filesystem::path GetCacheDirectory()
{
    static auto _ = (HZ_CORE_INFO("Initial Cache directory: {}", FPath(cache_relative_path).absolute_path.string()), 0);
    return FPath(cache_relative_path);
}

static void CreateCacheDirectoryIfNeeded()
{
    auto cache_dir = GetCacheDirectory();

    if (!std::filesystem::exists(cache_dir))
    {
        /*
        auto project_root = ::utils::ProjectRoot();
        auto dir_iterator = std::move(project_root);
        auto dirs         = ::utils::string_split(cache_relative_path, '/');
        for (auto dir : dirs) {
            auto new_dir = dir_iterator / dir;
            if (!std::filesystem::exists(new_dir)) {
                std::filesystem::create_directory(new_dir);
                dir_iterator = std::move(new_dir);
            }
        }
        */
        std::filesystem::create_directories(cache_dir);
    }
}

static const char *GLShaderStage_CachedOpenGL_FileExtension(GLenum stage)
{
    switch (stage)
    {
        case GL_VERTEX_SHADER:
            return ".cached_opengl.vert";
        case GL_FRAGMENT_SHADER:
            return ".cached_opengl.frag";
    }
    HZ_CORE_ASSERT(false);
    return "";
}

static const char *GLShaderStage_CachedVulkan_FileExtension(GLenum stage)
{
    switch (stage)
    {
        case GL_VERTEX_SHADER:
            return ".cached_vulkan.vert";
        case GL_FRAGMENT_SHADER:
            return ".cached_vulkan.frag";
    }
    HZ_CORE_ASSERT(false);
    return "";
}


} // namespace utils

OpenGLShader::OpenGLShader(const std::string &shader_file_path)
{
    HZ_PROFILE_FUNCTION();

    m_FilePath = shader_file_path;
    m_ShaderID = 0;

    utils::CreateCacheDirectoryIfNeeded();


    // derive the shader name from the filename
    m_Name = std::filesystem::path(m_FilePath).stem().string();
    HZ_CORE_ASSERT(!m_Name.empty(), "Cannot derive the shader name from shader file.");

    HZ_CORE_TRACE("Creating OpenGL Shader....\n\tShader file path: {} \n\tShader name: {}", m_FilePath.string(), m_Name);

    // check the source file hash
    bool bSourceChanged = true;

    auto source = ::utils::File::read_all(m_FilePath.string());
    HZ_CORE_ASSERT(source.has_value(), "Failed to read shader source file: {}", m_FilePath.string());

    auto hash = ::utils::File::get_hash(std::ref(source.value())).value();
    HZ_CORE_INFO("Current Shader hash: {}", hash);

    nlohmann::json cached_meta_json;
    auto           cached_meta_filepath = GetCacheMetaPath();
    std::ifstream  ifs(cached_meta_filepath);
    if (!ifs.fail()) {
        cached_meta_json = nlohmann::json::parse(ifs);
        if (cached_meta_json.contains("hash")) {
            HZ_CORE_ASSERT(cached_meta_json["hash"].is_number(), "Old shader hash is not a number");
            auto old_hash  = cached_meta_json["hash"].get<size_t>();
            bSourceChanged = (hash != old_hash);
            HZ_CORE_INFO("Cached shader hash:  {}", old_hash);
        }
    }

    // split shader sources
    std::unordered_map<GLenum, std::string> shader_sources;
    if (bSourceChanged) {
        shader_sources = PreProcess(source.value());
        // HZ_CORE_ERROR("{}:\n{}", utils::GLShaderStageToString(GL_VERTEX_SHADER), shader_sources[GL_VERTEX_SHADER]);
        // HZ_CORE_INFO("{}:\n{}", utils::GLShaderStageToString(GL_FRAGMENT_SHADER), shader_sources[GL_FRAGMENT_SHADER]);
    }
    else {
        // construct  empty shader sources for recompilation
        // TODO: remove  this  and fix successor steps
        shader_sources = {
            {  GL_VERTEX_SHADER, {}},
            {GL_FRAGMENT_SHADER, {}}
        };
    }


    {
        Timer timer;
        CreateVulkanBinaries(shader_sources, bSourceChanged);
        CreateGLBinaries(bSourceChanged);
        CreateProgram();
        // Sleep(3000);
        HZ_CORE_WARN("Shader compile and creation took {0} ms", timer.ElapsedMillis());
    }

    // overwrite the source hash
    if (bSourceChanged) {
        cached_meta_json["hash"] = hash;
        std::ofstream ofs(cached_meta_filepath, std::ios::out | std::ios::trunc);
        ofs << cached_meta_json.dump(4) << std::endl;
    }
}

OpenGLShader::OpenGLShader(const std::string &name, const std::string &vert_src, const std::string &frag_src)
{
    HZ_PROFILE_FUNCTION();


    m_ShaderID = 0;
    m_Name     = name;

    std::unordered_map<GLenum, std::string> sources = {
        {  GL_VERTEX_SHADER, vert_src},
        {GL_FRAGMENT_SHADER, frag_src}
    };

    CreateVulkanBinaries(sources, true);
    CreateGLBinaries(true);
    CreateProgram();
}

OpenGLShader::~OpenGLShader()
{
}

void OpenGLShader::Bind() const
{
    glUseProgram(m_ShaderID);
}
void OpenGLShader::Unbind() const
{
    glUseProgram(0);
}

void OpenGLShader::UploadUniformMat4(const std::string &name, const glm::mat4 &matrix)
{
    // transpose: A^t
    glUniformMatrix4fv(glGetUniformLocation(m_ShaderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}
void OpenGLShader::UploadUniformFloat4(const std::string &name, const glm::vec4 &float4)
{
    //    glUniform4fv(glGetUniformLocation(m_ShaderID, name.c_str()), 1, glm::value_ptr(float4));
    glUniform4f(glGetUniformLocation(m_ShaderID, name.c_str()), float4.x, float4.y, float4.z, float4.z);
}
void OpenGLShader::UploadUniformFloat(const std::string &name, const float value)
{
    glUniform1f(glGetUniformLocation(m_ShaderID, name.c_str()), value);
}
void OpenGLShader::UploadUniformFloat2(const std::string &name, const glm::vec2 &values)
{
    glUniform2f(glGetUniformLocation(m_ShaderID, name.c_str()), values.x, values.y);
}
void OpenGLShader::UploadUniformFloat3(const std::string &name, const glm::vec3 &values)
{
    GL_CALL(glUniform3f(glGetUniformLocation(m_ShaderID, name.c_str()), values.x, values.y, values.z));
}
void OpenGLShader::UploadUniformInt(const std::string &name, const int32_t value)
{
    // GL_CALL(
    glUniform1i(glGetUniformLocation(m_ShaderID, name.c_str()), value) /*)*/;
}

void OpenGLShader::UploadUniformIntArray(const std::string &name, const int32_t *value, uint32_t count)
{
    glUniform1iv(glad_glGetUniformLocation(m_ShaderID, name.c_str()), count, value);
}

std::string OpenGLShader::ReadFile(const std::string &shader_file_path)
{
    HZ_PROFILE_FUNCTION();

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
    HZ_PROFILE_FUNCTION();

    std::unordered_map<GLenum, std::string> shader_sources;

    const char  *type_token     = "#type";
    const size_t type_token_len = strlen(type_token);
    size_t       pos            = source.find(type_token, 0);

    while (pos != std::string ::npos)
    {
        // get the type string
        size_t eol = source.find_first_of(eol_flag, pos);
        HZ_CORE_ASSERT(eol != std::string ::npos, "Syntax error");
        size_t      begin = pos + type_token_len + 1;
        std::string type  = source.substr(begin, eol - begin);

        GLenum shader_type = utils::ShaderTypeFromString(type);
        HZ_CORE_ASSERT(shader_type, "Invalid shader type specific");

        // get the shader content range
        size_t next_line_pos = source.find_first_not_of(eol_flag, eol);

        pos          = source.find(type_token, next_line_pos);
        size_t count = (next_line_pos == std::string ::npos ? source.size() - 1 : next_line_pos);

        auto codes = source.substr(next_line_pos, pos - count);

        auto [_, Ok] = shader_sources.insert({(unsigned int)shader_type, codes});

        HZ_CORE_ASSERT(Ok, "Failed to insert this shader source");
    }

    return shader_sources;
}

void OpenGLShader::CreateVulkanBinaries(const std::unordered_map<unsigned int, std::string> &shader_sources, bool bSourceChanged)
{
    HZ_PROFILE_FUNCTION();

    shaderc::Compiler       compiler;
    shaderc::CompileOptions options;
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
    const bool optimize = true;
    if (optimize) {
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
    }

    auto &shader_data = m_Vulkan_SPIRV;
    shader_data.clear();

    for (auto &&[stage, source] : shader_sources)
    {
        auto shader_file_path = m_FilePath;
        auto cached_path      = GetCachePath(true, stage);

        // load binary spirv caches
        if (!bSourceChanged) {
            std::ifstream f(cached_path, std::ios::in | std::ios::binary);
            HZ_CORE_ASSERT(f.is_open(), "Cached spirv file not found when source do not changed!!");
            f.seekg(0, std::ios::end);
            auto size = f.tellg();
            f.seekg(0, std::ios::beg);
            auto &data = shader_data[stage];
            data.resize(size / sizeof(uint32_t));
            f.read((char *)data.data(), size);
            f.close();
        }
        // recompile
        else {
            shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(
                source,
                utils::GLShaderStageToShaderRcType(stage),
                fmt::format("{} ({})", m_FilePath.string(), utils::GLShaderStageToString(stage)).c_str(),
                options);

            if (result.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                HZ_CORE_ERROR(result.GetErrorMessage());
                std::filesystem::remove(cached_path);
                HZ_CORE_ASSERT(false);
            }

            // store compile result into memory and cached file
            shader_data[stage] = std::vector<uint32_t>(result.begin(), result.end());
            std::ofstream ofs(cached_path, std::ios::out | std::ios::binary | std::ios::trunc);
            if (ofs.is_open()) {
                auto &data = shader_data[stage];
                ofs.write((char *)data.data(), data.size() * sizeof(uint32_t));
                ofs.flush();
                ofs.close();
            }
        }
    }

    for (auto &&[stage, data] : shader_data)
    {
        Reflect(stage, data);
    }
}

void OpenGLShader::CreateGLBinaries(bool bSourceChanged)
{

    shaderc::Compiler       compiler;
    shaderc::CompileOptions options;
    options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);

    const bool optimize = false;
    if (optimize) {
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
    }

    auto &shader_data = m_OpenGL_SPIRV;
    shader_data.clear();
    m_OpenGL_SourceCode.clear();

    for (auto &&[stage, spirv_binary] : m_Vulkan_SPIRV)
    {
        auto cached_filepath = GetCachePath(false, stage);
        if (!bSourceChanged) {
            // load binary opengl caches
            std::ifstream in(cached_filepath, std::ios::in | std::ios::binary);
            HZ_CORE_ASSERT(in.is_open(), "Cached opengl file not found when source do not changed!!");

            in.seekg(0, std::ios::end);
            auto size = in.tellg();
            in.seekg(0, std::ios::beg);

            auto &data = shader_data[stage];
            data.resize(size / sizeof(uint32_t));
            in.read((char *)data.data(), size);
            in.close();
        }
        else
        {
            // spirv binary -> glsl source code
            spirv_cross::CompilerGLSL glsl_compiler(spirv_binary);
            m_OpenGL_SourceCode[stage] = glsl_compiler.compile();

            // HZ_CORE_INFO("????????????????????????\n{}", m_OpenGL_SourceCode[stage]);
            auto &source = m_OpenGL_SourceCode[stage];
            // HZ_CORE_ERROR(source);
            // HZ_CORE_ERROR(m_FilePath.string());

            shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(
                source,
                utils::GLShaderStageToShaderRcType(stage),
                m_FilePath.string().c_str());

            if (result.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                HZ_CORE_ERROR(result.GetErrorMessage());
                HZ_CORE_ASSERT(false);
            }

            // store compile result(opengl) into memory and cached file
            shader_data[stage] = std::vector<uint32_t>(result.cbegin(), result.cend());
            std::ofstream out(cached_filepath, std::ios::out | std::ios::binary | std::ios::trunc);
            if (out.is_open())
            {
                auto &data = shader_data[stage];
                out.write((char *)data.data(), data.size() * sizeof(uint32_t));
                out.flush();
                out.close();
            }
        }
    }
}

void OpenGLShader::CreateProgram()
{
    GLuint program = glCreateProgram();

    std::vector<GLuint> shader_ids;
    for (auto &&[stage, spirv] : m_OpenGL_SPIRV)
    {
        GLuint shader_id = shader_ids.emplace_back(glCreateShader(stage));
        // binary from SPIR-V
        glShaderBinary(1,
                       &shader_id,
                       GL_SHADER_BINARY_FORMAT_SPIR_V,
                       spirv.data(),
                       spirv.size() * sizeof(uint32_t));
        glSpecializeShader(shader_id, "main", 0, nullptr, nullptr);
        glAttachShader(program, shader_id);
    }

    glLinkProgram(program);

    GLint bLinked;
    glGetProgramiv(program, GL_LINK_STATUS, &bLinked);

    // error handle
    if (bLinked == GL_FALSE)
    {
        GLint max_len;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_len);

        std::vector<GLchar> info_log(max_len);
        glGetProgramInfoLog(program, max_len, &max_len, info_log.data());

        HZ_CORE_ERROR("Shader linking failed ({0}):\n{1}", m_FilePath.string(), info_log.data());

        glDeleteProgram(program);

        for (auto id : shader_ids) {
            glDeleteShader(id);
        }
    }

    // already linked
    for (auto id : shader_ids)
    {
        glDetachShader(program, id);
        glDeleteShader(id);
    }

    m_ShaderID = program;
}

void OpenGLShader::Reflect(GLenum stage, const std::vector<uint32_t> &shader_data)
{

    spirv_cross::Compiler        compiler(shader_data);
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    HZ_CORE_TRACE("OpenGLShader:Reflect  - {} {}", utils::GLShaderStageToString(stage), m_FilePath.string());
    HZ_CORE_TRACE("\t {} uniform buffers ", resources.uniform_buffers.size());
    HZ_CORE_TRACE("\t {} resources ", resources.sampled_images.size());

    HZ_CORE_TRACE("Uniform buffers:");
    for (const auto &resource : resources.uniform_buffers)
    {
        const auto &buffer_type  = compiler.get_type(resource.base_type_id);
        uint32_t    bufferSize   = compiler.get_declared_struct_size(buffer_type);
        uint32_t    binding      = compiler.get_decoration(resource.id, spv::DecorationBinding);
        int         member_count = buffer_type.member_types.size();

        HZ_CORE_TRACE("  {0}", resource.name);
        HZ_CORE_TRACE("\tSize = {0}", bufferSize);
        HZ_CORE_TRACE("\tBinding = {0}", binding);
        HZ_CORE_TRACE("\tMembers = {0}", member_count);
    }
}

std::filesystem::path OpenGLShader::GetCachePath(bool bVulkan, GLenum stage)
{
    auto cached_dir = utils::GetCacheDirectory();
    auto filename   = m_FilePath.filename().string() +
                    (bVulkan
                         ? utils::GLShaderStage_CachedVulkan_FileExtension(stage)
                         : utils::GLShaderStage_CachedOpenGL_FileExtension(stage));
    return cached_dir / filename;
}

std::filesystem::path OpenGLShader::GetCacheMetaPath()
{
    return utils::GetCacheDirectory() / (m_FilePath.filename().string() + ".cached.meta.json");
}


void OpenGLShader::SetInt(const std::string &name, const int32_t value)
{
    UploadUniformInt(name, value);
}

void OpenGLShader::SetIntArray(const std::string &name, int32_t *value, uint32_t count)
{
    UploadUniformIntArray(name, value, count);
}

void OpenGLShader::SetFloat3(const std::string &name, const glm::vec3 &values)
{
    UploadUniformFloat3(name, values);
}
void OpenGLShader::SetFloat4(const std::string &name, const glm::vec4 &float4)
{
    UploadUniformFloat4(name, float4);
}
void OpenGLShader::SetMat4(const std::string &name, const glm::mat4 &matrix)
{
    UploadUniformMat4(name, matrix);
}

void OpenGLShader::SetFloat(const std::string &name, const float value)
{
    UploadUniformFloat(name, value);
}


} // namespace hazel