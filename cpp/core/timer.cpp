#include <core/timer.h>

#include <GLFW/glfw3.h>

namespace gl {

    static float pBeginTime = 0;
    static float pDeltaTime = 6;

    void Timer::begin() {
        pBeginTime = glfwGetTime();
    }

    void Timer::end() {
        pDeltaTime = ((float)glfwGetTime() - pBeginTime) * 1000;
    }

    float Timer::getBeginMillis() {
        return pBeginTime;
    }

    float Timer::getDeltaMillis() {
        return pDeltaTime;
    }

    float Timer::getFPS() {
        return 1000.0f / pDeltaTime;
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