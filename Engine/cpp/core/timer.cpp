#include <core/timer.h>

#include <GLFW/glfw3.h>

namespace gl {

    Time Timer::s_beginTime = 0;
    Time Timer::s_deltaTime = 6;

    void Timer::begin() {
        s_beginTime = glfwGetTime();
    }

    void Timer::end() {
        s_deltaTime = ((float)glfwGetTime() - s_beginTime) * 1000;
    }

    Time Timer::getBeginMillis() {
        return s_beginTime;
    }

    Time Timer::getDeltaMillis() {
        return s_deltaTime;
    }

    Time Timer::getFPS() {
        return 1000.0f / s_deltaTime;
    }

    std::string Timer::getCurrentDateTime() {
        time_t rawtime;
        struct tm* timeinfo;
        char buffer[80];

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, sizeof(buffer),"%d-%m-%Y %H:%M:%S", timeinfo);
        return std::string(buffer);
    }

}