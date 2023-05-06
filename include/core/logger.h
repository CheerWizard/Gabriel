#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
#include <Windows.h>

using namespace std;

enum ConsoleColor {
    DARKBLUE = 1,
    DARKGREEN,
    DARKTEAL,
    DARKRED,
    DARKPINK,
    DARKYELLOW,
    GRAY,
    DARKGRAY,
    BLUE,
    GREEN,
    TEAL,
    RED,
    PINK,
    YELLOW,
    WHITE
};

#endif

struct Logger final {

    ConsoleColor verboseColor = ConsoleColor::WHITE;
    ConsoleColor infoColor = ConsoleColor::GREEN;
    ConsoleColor warnColor = ConsoleColor::YELLOW;
    ConsoleColor errorColor = ConsoleColor::RED;

    Logger(const char* filename);
    ~Logger();

    static Logger& get() {
        return *sInstance;
    }

    void print_verbose(const std::string& msg);
    void print_info(const std::string& msg);
    void print_warning(const std::string& msg);
    void print_error(const std::string& msg);

private:
    void print_out(ConsoleColor color, const std::string& msg);

private:
    static Logger* sInstance;
    FILE* mFile;
    HANDLE mOut;
    ConsoleColor mCurrentColor = verboseColor;
};

#define verbose(msg) \
{ \
    std::stringstream ss; \
    ss << __FUNCTION__ << ": " << msg << std::endl; \
    Logger::get().print_verbose(ss.str()); \
}

#define info(msg) \
{ \
    std::stringstream ss; \
    ss << __FUNCTION__ << ": " << msg << std::endl; \
    Logger::get().print_info(ss.str()); \
}

#define warning(msg) \
{ \
    std::stringstream ss; \
    ss << __FUNCTION__ << ": " << msg << std::endl; \
    Logger::get().print_warning(ss.str()); \
}

#define error(msg) \
{ \
    std::stringstream ss; \
    ss << __FUNCTION__ << ": " << msg << std::endl << "Error code line: " << __LINE__ << std::endl; \
    Logger::get().print_error(ss.str()); \
}

#define error_trace(msg, file, function, line) \
{ \
    std::stringstream ss; \
    ss << function << ": " << msg << std::endl << \
    "File: " << file << std::endl << \
    "Function: " << function << " line: " << line << std::endl; \
    Logger::get().print_error(ss.str()); \
}

#define print_fps(deltaTime) \
info("Delta time: " << (deltaTime) << " ms" << " FPS: " << 1000 / (deltaTime))