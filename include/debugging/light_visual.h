#pragma once

#include <features/transform.h>

#include <geometry/cube.h>

namespace gl {

    component(LightVisual) {
        glm::vec4 color = { 1, 1, 1, 1 };
        Transform transform = {
                { 0, 0, -2 },
                { 0, 0, 0 },
                { 0.1, 0.1, 0.1 },
        };
        DrawableElements drawable;
    };

    struct LightVisualRenderer final {

        LightVisualRenderer();
        ~LightVisualRenderer();

        void begin();
        void render(LightVisual& lightVisual);

    private:
        Shader mShader;
    };

}