#pragma once

#include <shader.h>
#include <draw.h>
#include <transform.h>
#include <component.h>

namespace gl {

    component(NormalVisual) {
        glm::vec3 color = { 1, 1, 0 };
        float length = 0.1f;
    };

    struct NormalVisualRenderer final {

        void init();
        void free();

        void begin();

        void render(NormalVisual& normal_visual, Transform& transform, DrawableElements& drawable);

    private:
        Shader shader;
    };

}