#pragma once


#include "__microshit_api_hazel.h"
#include <spdlog/spdlog.h>


#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

#include <source_location>

template <typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream &operator<<(OStream &os, const glm::vec<L, T, Q> &vector)
{
    return os << glm::to_string(vector);
}

template <typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream &operator<<(OStream &os, const glm::mat<C, R, T, Q> &matrix)
{
    return os << glm::to_string(matrix);
}

template <typename OStream, typename T, glm::qualifier Q>
inline OStream &operator<<(OStream &os, glm::qua<T, Q> quaternion)
{
    return os << glm::to_string(quaternion);
}



namespace hazel {


class HAZEL_API Log
{

  public:
    inline static std::shared_ptr<spdlog::logger> &GetCoreLogger()
    {
        return Get().core_logger;
    }

    inline static std::shared_ptr<spdlog::logger> &GetClientLogger()
    {
        return Get().client_logger;
    }


  private:
    inline static Log &Get()
    {
        static Log *log = new Log();
        return *log;
    }

    Log();
    ~Log();

    std::shared_ptr<spdlog::logger> core_logger;
    std::shared_ptr<spdlog::logger> client_logger;
};

} // namespace hazel

#define ___VA_NARGS_IMPL(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define VA_NARGS(...) ___VA_NARGS_IMPL(_, ##__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

// #define WITH_SOURCE_LOCATION(call, ...)                                          \
//     auto loc = std::source_location::current();                                  \
//     if constexpr (VA_NARGS(__VA_ARGS__) > 1) {                                   \
//         call(std::format("{}:{}", loc.file_name(), loc.line()), __VA_ARGS__);    \
//     }                                                                            \
//     else {                                                                       \
//         call(std::format("{}:{} {}", loc.file_name(), loc.line(), __VA_ARGS__)); \
//     }


#define HZ_CORE_TRACE(...) ::hazel::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define HZ_CORE_INFO(...) ::hazel::Log::GetCoreLogger()->info(__VA_ARGS__)
#define HZ_CORE_WARN(...) ::hazel::Log::GetCoreLogger()->warn(__VA_ARGS__)
// #define HZ_CORE_ERROR(...) WITH_SOURCE_LOCATION(::hazel::Log::GetCoreLogger()->error, __VA_ARGS__)
#define HZ_CORE_ERROR(...) ::hazel::Log::GetCoreLogger()->error(__VA_ARGS__)
// #define HZ_CORE_FATAL(...) ::hazel::Log::GetCoreLogger()->fatal(__VA_ARGS__)


#define HZ_TRACE(...) ::hazel::Log::GetClientLogger()->trace(__VA_ARGS__)
#define HZ_INFO(...) ::hazel::Log::GetClientLogger()->info(__VA_ARGS__)
#define HZ_WARN(...) ::hazel::Log::GetClientLogger()->warn(__VA_ARGS__)
// #define HZ_ERROR(...) WITH_SOURCE_LOCATION(::hazel::Log::GetClientLogger()->error, __VA_ARGS__)
#define HZ_ERROR(...) ::hazel::Log::GetClientLogger()->error(__VA_ARGS__)
// #define HZ_FATAL(...) ::hazel::Log::GetClientLogger()->fatal(__VA_ARGS__)
