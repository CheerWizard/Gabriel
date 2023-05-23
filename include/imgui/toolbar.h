#pragma once

#include <api/image.h>

namespace gl {

    struct ViewportsMenu final {
        static void render();
    };

    struct Viewports final {
        static void render();
    };

    struct Toolbar final {
        static void render();

    private:
        static void end();

    private:
        static bool sInitialized;
        static int sX, sY;
        static int sWidth, sHeight;
    };

}