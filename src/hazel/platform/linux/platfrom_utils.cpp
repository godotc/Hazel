#include "hz_pch.h"

#include "hazel/core/app.h"
#include "hazel/utils/platform_utils.h"


#include <filesystem>
#include <string>


namespace hazel {

std::string FileDialogs::OpenFile(const char *filter, std::filesystem::path initial_path)
{
    return {};
}

std::string FileDialogs::SaveFile(const char *filter, std::filesystem::path initial_path)
{
    return {};
}

} // namespace hazel
