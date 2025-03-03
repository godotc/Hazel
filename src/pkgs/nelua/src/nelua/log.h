

#pragma once

#include <cstdio>
#include <source_location>
#include <string>

inline void log_impl(const char *fmt, std::source_location loc)
{
    std::string      format   = std::string("%s:%d: ") + fmt + "\n";
    std::string_view filename = loc.file_name();

    static int n = []() {
        int              ret      = 0;
        std::string_view filename = std::source_location::current().file_name();
        n                         = filename.find("Hazel");
        return n;
    }();
    filename.remove_prefix(n);

    printf(format.c_str(), filename.data(), loc.line());
}

template <typename... Ts>
inline void log_impl(const char *fmt, std::source_location loc, Ts... ts)
{
    std::string      format   = std::string("%s:%d: ") + fmt + "\n";
    std::string_view filename = loc.file_name();

    static int n = []() {
        int              ret      = 0;
        std::string_view filename = std::source_location::current().file_name();
        n                         = filename.find("Hazel");
        return n;
    }();
    filename.remove_prefix(n);

    printf(format.c_str(), filename.data(), loc.line(), std::forward<Ts>(ts)...);
}

#define Log(fmt, ...) log_impl(fmt, std::source_location::current() __VA_OPT__(, ) __VA_ARGS__);