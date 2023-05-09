#pragma once

#include <core/common.h>

#ifdef DEBUG
    #define debugCheckError() gl::Debugger::checkError(__FILE__, __FUNCTION__, __LINE__)
#else
    #define debugCheckError()
#endif

namespace gl {

    struct Debugger final {

        Debugger();
        ~Debugger();

        static Debugger& get() {
            return *sInstance;
        }

        static bool checkError(const char* file, const char* function, int line);

    private:
        static Debugger* sInstance;
    };

}