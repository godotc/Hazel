#pragma once

#include "assert.h"
#include <source_location>



#ifndef TOPLEVEL_NAMESPACE
    #define TOPLEVEL_NAMESPACE reflect
#endif

#define TOPLEVEL_NAMESPACE_BEGIN namespace TOPLEVEL_NAMESPACE {
#define TOPLEVEL_NAMESPACE_END }

#define WITH_TEST 1

#if _WIN32
    #define __FUNCTION_SIG __FUNCSIG__
#elif __linux__
    #define __FUNCTION_SIG __PRETTY_FUNCTION__
#else
    #error Need your implementation
#endif

// 定义字符串转换工具宏
#define TO_STRING_IMPL(x) #x
#define TO_STRING(x) TO_STRING_IMPL(x)

// 根据不同编译器生成编译警告
#if defined(__GNUC__) || defined(__clang__)
    #define COMPILER_WARNING(msg) _Pragma(TO_STRING(message(msg)))
#elif defined(_MSC_VER)
    #define COMPILER_WARNING(msg) __pragma(message(msg))
#else
    #define COMPILER_WARNING(msg)
#endif

// 最终的 UNIMPLEMENTED 宏
#define UNIMPLEMENTED()                                                                  \
    do {                                                                                 \
        COMPILER_WARNING("WARNING: UNIMPLEMENTED at " __FILE__ ":" TO_STRING(__LINE__)); \
        fprintf(stderr, "TODO unimplemented: %s:%d\n", __FILE__, __LINE__);              \
        assert(0);                                                                       \
    } while (0)



#ifdef WIN32
    #define __FUCNTION_NAME__ __FUNCSIG__
#else
    #define __FUNCITON_NAME__ __PRETTY_FUNCTION__
#endif



#include <iostream>
#include <optional>
#include <ostream>


template <typename T>
inline std::ostream &operator<<(std::ostream &out, std::optional<T> v)
{
    if (v.has_value()) {
        out << v.value();
    }
    else {
        out << "[None]";
    }
    return out;
}



struct debug {
    std::ostream &out = std::cout;

    debug(std::source_location loc = std::source_location::current())
    {
        out << loc.file_name() << ":" << loc.line() << " | ";
    }
    template <typename Arg>
    debug &operator,(Arg arg)
    {
        out << arg;
        return *this;
    }
    ~debug() { out << '\n'; }
};
// static auto d = debug{};
