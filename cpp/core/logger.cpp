#include <core/logger.h>

#include <mutex>

FILE* Logger::sFile = NULL;
std::string Logger::sName;
HANDLE Logger::sOut = GetStdHandle(STD_OUTPUT_HANDLE);
ConsoleColor Logger::sCurrentColor = Logger::VERBOSE_COLOR;

void Logger::init(const char* filename) {
    std::stringstream ss;
    ss << filename << ".log";
    sName = ss.str();
    fopen_s(&sFile, sName.c_str(), "w");
}

void Logger::free() {
    if (sFile)
        fclose(sFile);
}

void Logger::printVerbose(const std::string& msg) {
    printOut(VERBOSE_COLOR, msg);
}

void Logger::printInfo(const std::string &msg) {
    printOut(INFO_COLOR, msg);
}

void Logger::printWarning(const std::string& msg) {
    printOut(WARN_COLOR, msg);
}

void Logger::printError(const std::string& msg) {
    printOut(ERROR_COLOR, msg);
}

void Logger::printOut(ConsoleColor color, const std::string& msg) {
    if (sCurrentColor != color) {
        sCurrentColor = color;
        SetConsoleTextAttribute(sOut, color);
    }

    time_t t = time(NULL);
    struct tm tm {};
    localtime_s(&tm, &t);
    char timeString[80];
    strftime(timeString, sizeof(timeString), "[%Y-%m-%d][%X]", &tm);

    std::stringstream logBuilder;
    logBuilder << timeString << "[Thread-" << std::this_thread::get_id() << ']' << msg;
    std::string log = logBuilder.str();

    std::cout << log << std::endl;

    if (sFile == NULL) {
        fopen_s(&sFile, sName.c_str(), "w");
    } else {
        fwrite(log.data(), sizeof(char), log.length(), sFile);
        fflush(sFile);
    }
}
