#pragma once

#include <api/draw.h>

#include <features/transform.h>

namespace gl {

    component(PolygonVisual) {
        glm::vec3 color = { 1, 1, 1 };
        float thickness = 1.0005f;
    };

    struct PolygonVisualRenderer final {

        PolygonVisualRenderer();
        ~PolygonVisualRenderer();

        void begin();
        void end();

        void render(PolygonVisual& polygonVisual, Transform& transform, DrawableElements& drawable);

    private:
        Shader mShader;
    };

}