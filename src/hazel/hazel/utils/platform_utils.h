#pragma once

#include "__microshit_api_hazel.h"
#include <string>
namespace hazel {

class HAZEL_API FileDialogs
{

  public:
    // retrun the filepath, cancel when empty string
    static std::string OpenFile(const char *filter);
    // retrun the filepath, cancel when empty string
    static std::string SaveFile(const char *filter);
};


} // namespace hazel