#pragma once

#include <primitives.h>

#ifdef DEBUG
    #define debug_check_error() gl::Debugger::check_error(__FILE__, __FUNCTION__, __LINE__)
#else
    #define debug_check_error()
#endif

namespace gl {

    struct Debugger final {

        static void init();
        static bool check_error(const char* file, const char* function, int line);

    };

}