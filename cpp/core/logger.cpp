#include <core/logger.h>

#include <sstream>

static Ref<spdlog::logger> pLogger;
static Ref<spdlog::logger> pTracer;
// [DateTime][Hours:Minutes:Seconds:Milliseconds]: Message
static const char* pLogPattern = "[%D][%H:%M:%S.%e]: %^%v%$";
// [DateTime][Hours:Minutes:Seconds:Milliseconds] FunctionName(FileName:CodeLine) Message
static const char* pTracePattern = "[%D][%H:%M:%S.%e] %^%!(%s:%#) %v%$";

static Ref<spdlog::logger> createLogger(const char* logName, const char* filepath, const char* pattern, int backtrace) {
    auto consoleSink = createRef<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_pattern(pattern);

    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filepath, true);
    fileSink->set_pattern(pattern);

    std::vector<spdlog::sink_ptr> sinks = { consoleSink, fileSink };

    auto logger = createRef<spdlog::logger>(logName, std::begin(sinks), std::end(sinks));
    spdlog::register_logger(logger);
    logger->enable_backtrace(backtrace);

    return logger;
}

void Logger::init(const char* name, int backtrace) {
    std::stringstream ss;
    try {
        ss << name << "Logger";
        std::string logName = ss.str();
        ss = {};
        ss << "logs/" << name << ".log";
        std::string filepath = ss.str();
        ss = {};
        pLogger = createLogger(logName.c_str(), filepath.c_str(), pLogPattern, backtrace);

        ss << name << "Tracer";
        logName = ss.str();
        ss = {};
        ss << "logs/" << name << ".trace";
        filepath = ss.str();
        ss = {};
        pTracer = createLogger(logName.c_str(), filepath.c_str(), pTracePattern, backtrace);
    }
    catch (const spdlog::spdlog_ex &ex) {
        SPDLOG_ERROR("Failed to initialize log {}. Error: {}", name, ex.what());
    }
}

void Logger::free() {
    pLogger.reset();
}

void Logger::dumpBacktrace() {
    pTracer->dump_backtrace();
}

spdlog::logger* Logger::getLogger() {
    return pLogger.get();
}

spdlog::logger* Logger::getTracer() {
    return pTracer.get();
}
