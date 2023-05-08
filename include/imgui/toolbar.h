#pragma once

#include <imgui/image_window.h>

#include <features/screen.h>

#include <postfx/hdr.h>
#include <postfx/blur.h>
#include <postfx/bloom.h>
#include <postfx/ssao.h>

namespace gl {

    struct ViewportsMenu final {
        static void render();
    };

    struct Viewports final {
        static void render();
    };

    struct Toolbar final {
        static void render();
    };

}