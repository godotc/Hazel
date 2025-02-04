//
/*
 * @ Author: godot42
 * @ Modified by: @godot42
 * @ Modified by: @godot42
 * @ Modified time: 2025-01-19 06:17:42
 * @ Description:
 */

#include "string_utils.h"
#include <cstddef>
#include <string_view>


std::string string_replace(const std::string &source, const std::string &from, const std::string &to)
{
    std::string ret = source;
    size_t      pos = 0;
    while ((pos = ret.find(from, pos)) != std::string::npos) {
        ret.replace(pos, from.size(), to);
        pos += to.size();
    }
    return ret;
}
namespace ut {
namespace str {


std::vector<std::string> split(std::string_view source, char delimiter)
{

    std::vector<std::string> ret;
    // "abc def"
    while (true) {
        // printf("source: %s\n", source.data());
        int n = source.find_first_of(delimiter);
        if (n == std::string::npos) {
            ret.emplace_back(source);
            break;
        }
        ret.emplace_back(source.substr(0, n));
        source.remove_prefix(n + 1);
    };
    return ret;
}

bool split(std::string_view source, char sep, std::string &left, std::string_view &right)
{
    int n = source.find_first_of(sep);
    if (n == std::string::npos) {
        left = source;
        return false;
    }
    left  = source.substr(0, n);
    right = source.substr(n + 1, source.size() - n - 1);
    return true;
}

bool left(std::string_view source, char delimiter, std::string_view &left)
{
    size_t n = source.find_first_of(delimiter);
    if (n != std::string::npos) {
        return false;
    }
    left = source.substr(0, n);
    return true;
}

} // namespace str

} // namespace ut
