/**
 *  Author: @godot42
 *  Create Time: 2024-03-07 00:55:14
 *  Modified by: @godot42
 *  Modified time: 2024-07-28 17:54:49
 *  Description:
 */


#pragma once

#include "__microshit_api_hazel.h"
#include <string>
namespace hazel {

class HAZEL_API FileDialogs
{

  public:
    // @return the filepath, cancel when empty string
    static std::string OpenFile(const char *filter);
    // @return the filepath, cancel when empty string
    static std::string SaveFile(const char *filter);
};


} // namespace hazel