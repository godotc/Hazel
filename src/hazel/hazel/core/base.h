//
/*
 * @ Author: godot42
 * @ Create Time: 2023-11-17 23:45:29
 * @ Modified by: @godot42
 * @ Modified time: 2025-03-02 18:49:26
 * @ Description:
 */

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
    #define HZ_ASSERT(x, ...)                                                \
        {                                                                    \
            if (!!!(x)) {                                                    \
                HZ_ERROR("Assertion Failed: {}" __VA_OPT__(, ) __VA_ARGS__); \
                PLATFORM_BREAK();                                            \
            }                                                                \
        }
    #define HZ_CORE_ASSERT(x, ...)                                                \
        {                                                                         \
            if (!!!(x)) {                                                         \
                HZ_CORE_ERROR("Assertion Failed: {}" __VA_OPT__(, ) __VA_ARGS__); \
                PLATFORM_BREAK();                                                 \
            }                                                                     \
        }
#else
    #define HZ_ASSERT(x, ...)
    #define HZ_CORE_ASSERT(x, ...)
#endif


#ifdef __HZ_DEBUG
    #define ENSURE(x) (!!(x) ? void(0) : HZ_ERROR("Assertion Failed: {}", #x), x)
#else
    #define ENSURE(x) x
#endif


#define BIT(x) (1 << x)
#define HZ_BIND_EVENT(ctx, fn) std::bind(fn, ctx, std::placeholders::_1)
#define HZ_BIND_EVENT_LAMBDA(fn) [this](auto &&...args) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace hazel {

template <class T>
using Scope = std::unique_ptr<T>;
template <class T, class... Args>
constexpr Scope<T> CreateScope(Args &&...args) { return std::make_unique<T>(std::forward<Args>(args)...); }

template <class T>
using Ref = std::shared_ptr<T>;
template <class T, class... Args>
constexpr Ref<T> CreateRef(Args &&...args) { return std::make_shared<T>(std::forward<Args>(args)...); }

template <class T>
using Weak = std::weak_ptr<T>;


}; // namespace hazel


/*
 * @param name
 * @param type
 */
#define NPROPERTY(...)

enum NPROPERTY
{


};

#define GENERATED_BODY(...)