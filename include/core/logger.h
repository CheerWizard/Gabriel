#pragma once

#include <core/pointers.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async.h>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

struct Logger final {

    static void init(const char* name, int backtrace);
    static void free();

    static spdlog::logger* getLogger();
    static spdlog::logger* getTracer();

    static void dumpBacktrace();

};

#ifdef DEBUG

#define trace(...) SPDLOG_LOGGER_TRACE(Logger::getTracer(), __VA_ARGS__)
#define verbose(...) SPDLOG_LOGGER_TRACE(Logger::getLogger(), __VA_ARGS__)
#define info(...) SPDLOG_LOGGER_INFO(Logger::getLogger(), __VA_ARGS__)
#define warning(...) SPDLOG_LOGGER_WARN(Logger::getTracer(), __VA_ARGS__)
#define error(...) SPDLOG_LOGGER_ERROR(Logger::getTracer(), __VA_ARGS__)
#define error_trace(...) \
error(__VA_ARGS__); \
Logger::dumpBacktrace()

#define printFPS(dt) \
info("Delta time: {} ms, FPS: {}", dt, 1000 / dt)

#define printVec4(name, v) \
info("{}:{}", name, glm::to_string(v))

#else

#define trace(...)
#define verbose(...)
#define info(...)
#define warning(...)
#define error(...)
#define error_trace(...)
#define printFPS(dt)
#define printVec4(name, v)

#endif