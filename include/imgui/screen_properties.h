#pragma once

#include <imgui/image_window.h>

#include <features/screen.h>

#include <postfx/hdr.h>
#include <postfx/blur.h>
#include <postfx/bloom.h>
#include <postfx/ssao.h>

namespace gl {

    struct ScreenProperties final {

        static const char* title;
        static glm::vec2 resolution;
        static glm::vec2 position;

        static ScreenRenderer* screenRenderer;
        static HdrRenderer* hdrRenderer;
        static BlurRenderer* blurRenderer;
        static BloomRenderer* bloomRenderer;
        static SsaoRenderer* ssaoRenderer;

        static void render();

    private:
        static ImageWindow screenImage;
        static ImageWindow hdrImage;
        static ImageWindow blurImage;
        static ImageWindow bloomImage;
        static ImageWindow ssaoImage;
    };

}