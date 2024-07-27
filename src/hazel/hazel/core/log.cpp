#include "log.h"
#include <iostream>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace hazel {



Log::Log()
{
    std::cout << fmt::format("Initializing the spd logger...") << std::endl;


    spdlog::set_pattern("%^[%T] %n: %v%$");
    // spdlog::set_pattern("%^[%l]%$ %v");

    core_logger = spdlog::stdout_color_mt("HAZEL");
    core_logger->set_level(spdlog::level::trace);

    client_logger = spdlog::stdout_color_mt("APP");
    client_logger->set_level(spdlog::level::trace);
}

Log::~Log()
{
}

}; // namespace hazel