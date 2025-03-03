#include "log.h"
#include <iostream>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace hazel {

class CustomFormater : public spdlog::formatter
{
    std::unique_ptr<spdlog::formatter> _formatterWithLine;
    std::unique_ptr<spdlog::formatter> _formatter;

  public:

    CustomFormater()
    {
        _formatter         = std::make_unique<spdlog::pattern_formatter>("%^[%T] %n : %v%$");
        _formatterWithLine = std::make_unique<spdlog::pattern_formatter>("%^[%T] %n [%L]: %v%$");
    }

    void format(const spdlog::details::log_msg &msg, spdlog::memory_buf_t &dest) override
    {

        // 如果是错误级别且存在源代码信息，则追加
        if (msg.level >= spdlog::level::warn) {
            _formatterWithLine->format(msg, dest);
            // spdlog::fmt_lib::format_to(std::back_inserter(dest),
            //                            " [{}:{}]",
            //                            msg.source.filename,
            //                            msg.source.line);
        }
        else {
            _formatter->format(msg, dest);
        }
    }

    std::unique_ptr<spdlog::formatter> clone() const override
    {
        auto cloned                = std::make_unique<CustomFormater>();
        cloned->_formatter         = _formatter->clone();
        cloned->_formatterWithLine = _formatterWithLine->clone();
        return cloned;
    }
};



Log::Log()
{
    std::cout << fmt::format("Initializing the spd logger...") << std::endl;


    // spdlog::set_pattern("%^[%T] %n: %v%$");
    // spdlog::set_pattern("%^[%l]%$ %v");
    spdlog::set_formatter(std::make_unique<CustomFormater>());

    core_logger = spdlog::stdout_color_mt("HAZEL");
    core_logger->set_level(spdlog::level::trace);

    client_logger = spdlog::stdout_color_mt("APP");
    client_logger->set_level(spdlog::level::trace);
}

Log::~Log()
{
}

}; // namespace hazel