//
// Created by nono on 10/7/23.
//

#ifndef HAZEL_PATH_H
#define HAZEL_PATH_H

#include <filesystem>

namespace hazel {

using path = std::filesystem::path;


inline static std::string        project_root_symbol = ".project-root-symbol";
inline static void               SetProjectRootSymbol(std::string symbol) { project_root_symbol = symbol; }
inline static const std::string &GetProjectRootSymbol() { return project_root_symbol; }

static path get_runtime_exe_path();

// I will find `bin`'s  parent dir as the root dir;
static path find_directory_by_file_symbol(std::string target_symbol);


struct FPath {
    static const std::string &ProjectRoot();

    FPath(const char *path);
    FPath(std::string path) : FPath(path.c_str()) {}
    FPath(std::string &path) : FPath(path.c_str()) {}

    operator const std::string &() { return absolute_path; }
    operator const char *() { return absolute_path.c_str(); }
    std::string absolute_path{};
};



} // namespace hazel

#endif // HAZEL_PATH_H
