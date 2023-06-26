#pragma once

namespace gl {

    struct GABRIEL_API Viewport final {
        static void resize(int x, int y, int w, int h);
    };

}