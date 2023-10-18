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

path get_runtime_exe_path();

// iterate parent and children recursively
path find_directory_by_file_symbol(path &initial_pos, std::string target_symbol);

const UTILS_API std::filesystem::path &ProjectRoot();

namespace impl {
struct UTILS_API FPathImpl {
    FPathImpl(const char *the_path)
    {
//        HZ_PROFILE_FUNCTION();
        absolute_path = ProjectRoot() / the_path;
    }

    operator const char *() const { return absolute_path.string().c_str(); }
    operator const char *&&() const { return std::move(absolute_path).string().c_str(); }

    [[nodiscard]] operator std::string() const { return absolute_path.string(); }
    //    const std::string &string() const { return absolute_path; }

    path absolute_path;
};
}; // namespace impl


} // namespace utils

using FPath = utils::impl::FPathImpl;
