#pragma once

#include <api/draw.h>

#include <features/transform.h>

namespace gl {

    component(NormalVisual) {
        glm::vec3 color = { 1, 1, 0 };
        float length = 0.1f;
    };

    struct GABRIEL_API NormalVisualRenderer final {

        NormalVisualRenderer();
        ~NormalVisualRenderer();

        void begin();

        void render(NormalVisual& normalVisual, Transform& transform, DrawableElements& drawable);

    private:
        Shader mShader;
    };

}