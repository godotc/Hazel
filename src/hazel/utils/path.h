//
// Created by nono on 10/7/23.
//

#pragma once

#include "__microshit_api_utils.h"


#include <filesystem>
#include <memory>


#ifndef UTILS_API
    #define UTILS_API
#endif

using path = std::filesystem::path;


namespace utils {

void               SetProjectRootSymbol(std::string symbol);
const std::string &GetProjectRootSymbol();
path               get_runtime_exe_path();

// iterate parent and children recursively
path find_directory_by_file_symbol(path &initial_pos, std::string target_symbol);


const UTILS_API std::filesystem::path &ProjectRoot();
namespace impl {
struct UTILS_API FPathImpl {
    explicit FPathImpl(const char *the_path)
    {
        // HZ_PROFILE_FUNCTION();
        absolute_path = ProjectRoot() / the_path;
    }

    operator const char *() const { return absolute_path.string().c_str(); }

    [[nodiscard]] operator std::string() const { return absolute_path.string(); }
    //    const std::string &string() const { return absolute_path; }

    path absolute_path;
};
}; // namespace impl



struct UTILS_API Files {
    static std::string GetFileNameWithoutExtension(const std::string &path);


    using file_filter = bool(const std::string &);
    static void ForeachFileInFolder(std::filesystem::path path, file_filter file_filter, auto &&predicate)
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
