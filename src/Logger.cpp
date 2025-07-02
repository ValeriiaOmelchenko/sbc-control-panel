#include "../includes/Logger.hpp"
#include <iostream>


std::shared_ptr<spdlog::logger> logger;

void Logger::init() {
    try {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::debug);

        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            "logs/sbc.log", 1024 * 1024 * 5, 3);  
        file_sink->set_level(spdlog::level::debug);

        std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};

        logger = std::make_shared<spdlog::logger>("main", begin(sinks), end(sinks));
        logger->set_level(spdlog::level::debug);
        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%s:%#] %v");

        spdlog::register_logger(logger);
        spdlog::set_default_logger(logger);
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
    }
}

std::shared_ptr<spdlog::logger> Logger::get() {
    return logger;
}
