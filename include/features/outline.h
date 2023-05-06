#pragma once

#include <api/draw.h>

#include <ecs/scene.h>

#include <control/camera.h>

#include <features/transform.h>

#include <unordered_map>

namespace gl {

    component(Outline) {
        glm::vec4 color = { 1, 1, 0, 1 };
        float thickness = 0.02f;
    };

    struct OutlineRenderer final {

        OutlineRenderer();
        ~OutlineRenderer();

        void begin();
        void end();

        void render(Outline& outline, Transform& transform, DrawableElements& drawable);

    private:
        Shader mShader;
    };

}