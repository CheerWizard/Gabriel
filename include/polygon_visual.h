#pragma once

#include <shader.h>
#include <draw.h>
#include <transform.h>
#include <component.h>

namespace gl {

    struct PolygonVisual : ecs::Component {
        glm::vec3 color = { 1, 1, 1 };
        float thickness = 1.0005f;
    };

    struct PolygonVisualRenderer final {

        void init();
        void free();

        void begin();
        void end();

        void render(PolygonVisual& polygon_visual, Transform& transform, DrawableElements& drawable);

    private:
        Shader shader;
    };

}