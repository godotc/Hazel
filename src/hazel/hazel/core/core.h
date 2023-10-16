#pragma once

#include "__microshit_api_hazel.h"

#include "log.h"
#include <memory>


#ifdef __HZ_DEBUG
    #define __HZ_ENABLE_ASSERTS
#endif

#if defined(_WIN32)
    #define PLATFORM_BREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
    #define PLATFORM_BREAK() __builtin_trap()
#else
    #define PLATFORM_BREAK()
#endif


#ifdef __HZ_ENABLE_ASSERTS
    #define HZ_ASSERT(x, ...)                                  \
        {                                                      \
            if (!!!(x)) {                                      \
                HZ_ERROR("Assertion Failed: {}", __VA_ARGS__); \
                PLATFORM_BREAK();                              \
            }                                                  \
        }
    #define HZ_CORE_ASSERT(x, ...)                                  \
        {                                                           \
            if (!!!(x)) {                                           \
                HZ_CORE_ERROR("Assertion Failed: {}", __VA_ARGS__); \
                PLATFORM_BREAK();                                   \
            }                                                       \
        }
#else
    #define HZ_ASSERT(x, ...)
    #define HZ_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)
#define HZ_BIND_EVENT(ctx, fn) std::bind(fn, ctx, std::placeholders::_1)

namespace hazel {

template <class T>
using Scope = std::unique_ptr<T>;


template <class T>
using Ref = std::shared_ptr<T>;

}; // namespace hazel