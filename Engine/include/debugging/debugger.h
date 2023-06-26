#pragma once

#ifdef DEBUG
    #define debugCheckError() gl::Debugger::checkError()
#else
    #define debugCheckError()
#endif

namespace gl {

    struct GABRIEL_API Debugger final {

        Debugger();
        ~Debugger();

        static Debugger& get() {
            return *sInstance;
        }

        static bool checkError();

    private:
        static Debugger* sInstance;
    };

}