#include <core/logger.h>

Logger* Logger::sInstance = nullptr;

Logger::Logger(const char* filename) {
    sInstance = this;
    mOut = GetStdHandle(STD_OUTPUT_HANDLE);
    std::stringstream ss;
    ss << filename << ".logs";
    std::string logName = ss.str();
    fopen_s(&mFile, logName.c_str(), "wb");
}

Logger::~Logger() {
    fclose(mFile);
}

void Logger::print_verbose(const std::string& msg) {
    print_out(verboseColor, msg);
}

void Logger::print_info(const std::string &msg) {
    print_out(infoColor, msg);
}

void Logger::print_warning(const std::string& msg) {
    print_out(warnColor, msg);
}

void Logger::print_error(const std::string& msg) {
    print_out(errorColor, msg);
}

void Logger::print_out(ConsoleColor color, const std::string& msg) {
    if (mCurrentColor != color) {
        mCurrentColor = color;
        SetConsoleTextAttribute(mOut, color);
    }

    std::cout << msg << std::endl;

    fwrite(msg.data(), sizeof(char), msg.length(), mFile);
}
