//
// Created by nono on 10/7/23.
//


#include "path.h"



path get_runtime_exe_path()
{
    // just not support the non-endlish path
    std::string path_str(4096, '\0');
#if _WIN32
    int len = GetModuleFileName(NULL, path_str.data(), path_str.size());
    if (len <= 0)
        return {};
#elif __linux__
    ssize_t n = readlink("/proc/self/exe", path_str.data(), path_str.size());
    if (n < 0)
        return {};
#elif __APPLE__
    int pid = GetCurrentProcessId();
    if (proc_pidpath(pid, path_str, path_str.size()) <= 0)
        return {};
#endif // ZEN_PLATFORM_WINDOWS
    return path_str;
}


static bool is_dir_contain_root_symbol(std::filesystem::path &the_path, std::string &target_symbol)
{
    for (auto &file : std::filesystem::directory_iterator(the_path))
    {
        if (file.path().filename() == target_symbol) {
            return true;
        }
    }
    return false;
}

path find_directory_by_file_symbol(std::string target_symbol)
{
    static path exe_path = get_runtime_exe_path();

    if (!exe_path.empty() &&
#if _WIN32
        exe_path.extension().compare(".exe") && // not work on linux
#endif
        exe_path.has_parent_path())
    {
        //        fmt::print("start iterating parent...");
        path directory = exe_path;
        while (
            directory.has_parent_path()
#if __linux__
            && directory.string() != "/"
#endif
        )
        {
            directory = directory.parent_path();
            if (is_dir_contain_root_symbol(directory, target_symbol)) {
                return directory;
            }
        };
    }

    throw std::runtime_error("The directory mark by symbol file not found! Symbol file: " + target_symbol);
}


const std::filesystem::path &ProjectRoot()
{
    static auto project_root = std::filesystem::absolute(find_directory_by_file_symbol(GetProjectRootSymbol()));
    return project_root;
}
