#pragma once

#include <api/draw.h>
#include <api/polygon_mode.h>

#include <features/transform.h>

namespace gl {

    component(PolygonVisual) {
        glm::vec3 color = { 0, 1, 0 };
        float thickness = 1.0005f;
    };

    struct PolygonVisualRenderer final {

        static PolygonFace polygonFace;
        static PolygonType polygonType;

        PolygonVisualRenderer();
        ~PolygonVisualRenderer();

        void begin();
        void end();

        void render(PolygonVisual& polygonVisual, Transform& transform, DrawableElements& drawable);

    private:
        Shader mShader;
    };

}