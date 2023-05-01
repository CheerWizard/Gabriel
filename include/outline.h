#pragma once

#include <scene.h>
#include <camera.h>
#include <transform.h>
#include <draw.h>

#include <unordered_map>

namespace gl {

    component(Outline) {
        glm::vec4 color = { 1, 1, 0, 1 };
        float thickness = 0.02f;
    };

    struct OutlineRenderer final {

        void init();
        void free();

        void begin();
        void end();

        void render(Outline& outline, Transform& transform, DrawableElements& drawable);

    private:
        Shader shader;
    };

}