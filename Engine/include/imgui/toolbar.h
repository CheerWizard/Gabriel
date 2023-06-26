#pragma once

#include <api/image.h>

namespace gl {

    struct GABRIEL_API ViewportsMenu final {
        static void render();
    };

    struct GABRIEL_API Viewports final {
        static void render();
    };

    struct Dockspace final {
        static void render();

    private:
        static void end();

        static void pollEvents();

    private:
        static bool sInitialized;
        static int sX, sY;
        static int sWidth, sHeight;
    };

    struct GABRIEL_API Toolbar final {
        static void render();

    private:
        static void drawLogo();
        static void drawFileMenu();
        static void drawWindowButtons();
    };

    struct GABRIEL_API MainWindow final {
        static void render();
    };

}