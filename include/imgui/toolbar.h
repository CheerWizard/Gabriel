#pragma once

#include <api/image.h>

namespace gl {

    struct ViewportsMenu final {
        static void render();
    };

    struct Viewports final {
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

    struct Toolbar final {
        static void render();

    private:
        static void drawLogo();
        static void drawFileMenu();
        static void drawWindowButtons();
    };

    struct MainWindow final {
        static void render();
    };

}