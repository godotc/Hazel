/*
 * @ Author: godot42
 * @ Create Time: 2025-01-06 21:14:30
 * @ Modified by: @godot42
 * @ Modified time: 2025-01-11 02:46:12
 * @ Modified time: 2025-01-19 05:07:31
 */


#pragma once

#include <string>
#include <string_view>
#include <vector>


namespace ut {

std::string string_replace(const std::string &source, const std::string &from, const std::string &to);


namespace str {
std::vector<std::string> split(std::string_view source, char delimiter = ' ');
bool                     split(std::string_view source, char sep, std::string &left, std::string_view &right);
std::string_view         left(std::string_view source, std::string_view delimiter);

} // namespace str


} // namespace ut