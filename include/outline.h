#pragma once

#include <scene.h>
#include <camera.h>
#include <transform.h>
#include <draw.h>

#include <unordered_map>

namespace gl {

    using namespace ecs;

    struct Outline : Component {
        Transform* transform;
        DrawableElements* drawable;
        float thickness = 0.02f;
        glm::vec4 color = { 1, 1, 0, 1 };

        Outline() = default;

        Outline(Transform* transform, DrawableElements* drawable)
        : transform(transform), drawable(drawable) {}
    };

    struct OutlineRenderer final {

        void init();
        void free();

        void begin();
        void end();

        void render(Outline* outline);

    private:
        Shader shader;
    };

}