#pragma once

#include <core/common.h>

namespace gl {

    struct Timer final {

        static void begin();
        static void end();

        static float getBeginMillis();
        static float getDeltaMillis();
        static float getFPS();
        static std::string getCurrentDateTime();

    };

}