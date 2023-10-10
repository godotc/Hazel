#pragma once

#include "core.h"

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>


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


    static std::shared_ptr<spdlog::logger> core_logger;
    static std::shared_ptr<spdlog::logger> client_logger;
};

} // namespace hazel


#define HZ_CORE_TRACE(...) ::hazel::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define HZ_CORE_INFO(...) ::hazel::Log::GetCoreLogger()->info(__VA_ARGS__)
#define HZ_CORE_WARN(...) ::hazel::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define HZ_CORE_ERROR(...) ::hazel::Log::GetCoreLogger()->error(__VA_ARGS__)
// #define HZ_CORE_FATAL(...) ::hazel::Log::GetCoreLogger()->fatal(__VA_ARGS__)


#define HZ_TRACE(...) ::hazel::Log::GetClientLogger()->trace(__VA_ARGS__)
#define HZ_INFO(...) ::hazel::Log::GetClientLogger()->info(__VA_ARGS__)
#define HZ_WARN(...) ::hazel::Log::GetClientLogger()->warn(__VA_ARGS__)
#define HZ_ERROR(...) ::hazel::Log::GetClientLogger()->error(__VA_ARGS__)
// #define HZ_FATAL(...) ::hazel::Log::GetClientLogger()->fatal(__VA_ARGS__)
