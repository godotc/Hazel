/**
 *  Author: @godot42
 *  Create Time: 2023-11-17 23:45:29
 *  Modified by: @godot42
 *  Modified time: 2024-07-30 15:08:59
 *  Description:
 */



#pragma once
#include "__microshit_api_utils.h"


#include <filesystem>


#ifndef UTILS_API
    #define UTILS_API
#endif



namespace utils {

using stdpath = std::filesystem::path;

void               SetProjectRootSymbol(std::string symbol);
const std::string &GetProjectRootSymbol();
stdpath            get_runtime_exe_path();

// iterate parent and children recursively
stdpath find_directory_by_file_symbol(stdpath &initial_pos, std::string target_symbol);


const UTILS_API std::filesystem::path &ProjectRoot();
namespace impl {
struct UTILS_API FPathImpl {
    explicit FPathImpl(const char *the_path)
    {
        // HZ_PROFILE_FUNCTION();
        absolute_path = ProjectRoot() / the_path;
        // absolute_path = absolute_path.lexically_normal();
    }

    explicit FPathImpl(const std::string &the_path)
    {
        // HZ_PROFILE_FUNCTION();
        absolute_path = ProjectRoot() / the_path;
        // absolute_path = absolute_path.lexically_normal();
    }

    // operator const char *() const { return absolute_path.string().c_str(); }

    [[nodiscard]] operator std::string() const { return absolute_path.string(); }
    //    const std::string &string() const { return absolute_path; }

    stdpath absolute_path;
};
}; // namespace impl



struct UTILS_API Files {
    static std::string GetFileNameWithoutExtension(const std::string &path);

    using file_filter = bool(const std::string &);

    template <class Predicate>
    static void ForeachFileInFolder(std::filesystem::path path, file_filter file_filter, Predicate &&predicate)
    {
        for (const auto texture : std::filesystem::directory_iterator(path))
        {
            const auto &file_path = texture.path().string();

            if (file_filter(file_path))
            {
                predicate(file_path);
            }
        }
    }
};

} // namespace utils

using FPath = utils::impl::FPathImpl;
