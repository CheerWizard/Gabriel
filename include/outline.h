#pragma once

#include <camera.h>
#include <transform.h>
#include <draw.h>

namespace gl {

    struct Outline final {
        float thickness = 0.02f;
        glm::vec4 color = { 1, 1, 0, 1 };

        void init();

        static void free();

        static void begin();

        static void end();

        void draw(Transform& transform, const DrawableElements& drawable);
    };

}