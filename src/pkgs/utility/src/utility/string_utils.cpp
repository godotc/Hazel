//
/*
 * @ Author: godot42
 * @ Modified by: @godot42
 * @ Modified by: @godot42
 * @ Modified time: 2025-01-11 03:32:35
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
    int                      nth = 0;
    // "abc def"
    while ((nth = source.find(delimiter)) != std::string::npos) {
        ret.emplace_back(source.substr(0, nth - 1));
        source.remove_prefix(nth + 1);
    }
    return ret;
}

bool split(std::string_view source, char sep, std::string_view &left, std::string_view &right)
{
    int n = source.find_first_of(sep);
    if (n == std::string::npos) {
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
