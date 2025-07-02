#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

class Logger {
public:
    static void init(); 
    static std::shared_ptr<spdlog::logger> get();
};
