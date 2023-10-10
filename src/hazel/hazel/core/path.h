//
// Created by nono on 10/7/23.
//

#ifndef HAZEL_PATH_H
#define HAZEL_PATH_H

#include <filesystem>
#include <memory>

namespace hazel {

using path = std::filesystem::path;


inline static std::string        project_root_symbol = ".project-root-symbol";
inline static void               SetProjectRootSymbol(std::string symbol) { project_root_symbol = symbol; }
inline static const std::string &GetProjectRootSymbol() { return project_root_symbol; }

static path get_runtime_exe_path();

// I will find `bin`'s  parent dir as the root dir;
static path find_directory_by_file_symbol(std::string target_symbol);

const std::filesystem::path &ProjectRoot();

namespace impl {

struct FPathImpl {
    FPathImpl(const char *the_path) { absolute_path = ProjectRoot() / the_path; }

    operator const char *() const { return absolute_path.c_str(); }

    // FIXME: method 2, only return the copy
    [[nodiscard]] operator std::string() const { return absolute_path; }
    //    const std::string &string() const { return absolute_path; }

    path absolute_path;
};
}; // namespace impl


// TODO: Extend the lifecycle in a function call for temp construct:
// m_ShaderLibrary->Load("hello", FPath("res/shader/texture.glsl"));
// m_Texture     = hazel::Texture2D::Create(FPath("res/texture/face.png"));
// m_ArchTexture = hazel::Texture2D::Create(FPath("res/texture/arch.png"));
//-----

// FIXME: method 1, direct using, depends on the std::string construct for using
// #define FPath(path) impl::FPathImpl(path).operator const char *()

using FPath = impl::FPathImpl;


// Deprecated:  use the path instanced, or on stack, no need ptr
// const char *a(std::shared_ptr<impl::FPathImpl> path)
//{
//     return path->operator const char *();
// }
//
// operator const char *() const { return absolute_path.c_str(); }
//
//[[nodiscard]] operator std::string &&() const { return absolute_path; }
// const std::string &string() const { return absolute_path; }
//
//
// std::shared_ptr<impl::FPathImpl> FPath(const char *path)
//{
//     return std::make_shared<impl::FPathImpl>(path);
// }
// std::shared_ptr<impl::FPathImpl> FPath(std::string &path) { return FPath(path.c_str()); }


} // namespace hazel

#endif // HAZEL_PATH_H
