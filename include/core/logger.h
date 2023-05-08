#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>
#include <mutex>

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

    static constexpr ConsoleColor VERBOSE_COLOR = ConsoleColor::WHITE;
    static constexpr ConsoleColor INFO_COLOR = ConsoleColor::GREEN;
    static constexpr ConsoleColor WARN_COLOR = ConsoleColor::YELLOW;
    static constexpr ConsoleColor ERROR_COLOR = ConsoleColor::RED;

    static void init(const char* filename);
    static void free();

    static void printVerbose(const std::string& msg);
    static void printInfo(const std::string& msg);
    static void printWarning(const std::string& msg);
    static void printError(const std::string& msg);

private:
    static void printOut(ConsoleColor color, const std::string& msg);

private:
    static FILE* sFile;
    static std::string sName;
    static HANDLE sOut;
    static ConsoleColor sCurrentColor;
};

#define verbose(msg) \
{ \
    std::stringstream ss; \
    ss << __FUNCTION__ << ": " << msg << std::endl; \
    Logger::printVerbose(ss.str()); \
}

#define info(msg) \
{ \
    std::stringstream ss; \
    ss << __FUNCTION__ << ": " << msg << std::endl; \
    Logger::printInfo(ss.str()); \
}

#define warning(msg) \
{ \
    std::stringstream ss; \
    ss << __FUNCTION__ << ": " << msg << std::endl; \
    Logger::printWarning(ss.str()); \
}

#define error(msg) \
{ \
    std::stringstream ss; \
    ss << __FUNCTION__ << " (" << __FILE__ << " line:" << __LINE__ << "): " << msg << std::endl; \
    Logger::printError(ss.str()); \
}

#define error_trace(msg, file, function, line) \
{ \
    std::stringstream ss; \
    ss << function << ": " << msg << std::endl << \
    "File: " << file << std::endl << \
    "Function: " << function << " line: " << line << std::endl; \
    Logger::printError(ss.str()); \
}

#define printFPS(deltaTime) \
info("Delta time: " << (deltaTime) << " ms" << " FPS: " << 1000 / (deltaTime))