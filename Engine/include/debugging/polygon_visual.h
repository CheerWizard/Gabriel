#pragma once

#include <api/draw.h>
#include <api/polygon_mode.h>

#include <features/transform.h>

namespace gl {

    struct GABRIEL_API DrawableVisual {
        DrawableElements* drawable;
        glm::vec3 color = { 0, 1, 0 };
        float thickness = 1.0005f;
    };

    component(Drawable2dVisual), DrawableVisual {
        Transform2d* transform;

        Drawable2dVisual() = default;

        Drawable2dVisual(
                Transform2d* transform,
                DrawableElements* drawable,
                const glm::vec3& color = { 0, 1, 0 },
                const float thickness = 1.0005f
        ) {
            this->transform = transform;
            this->drawable = drawable;
        }
    };

    component(Drawable3dVisual), DrawableVisual {
        Transform* transform;

        Drawable3dVisual() = default;

        Drawable3dVisual(
                Transform* transform,
                DrawableElements* drawable,
                const glm::vec3& color = { 0, 1, 0 },
                const float thickness = 1.0005f
        ) {
            this->transform = transform;
            this->drawable = drawable;
        }
    };

    component(Text2dVisual), DrawableVisual {
        Transform2d* transform;

        Text2dVisual() = default;

        Text2dVisual(
                Transform2d* transform,
                DrawableElements* drawable,
                const glm::vec3& color = { 0, 1, 0 },
                const float thickness = 1.0005f
        ) {
            this->transform = transform;
            this->drawable = drawable;
        }
    };

    component(Text3dVisual), DrawableVisual {
        Transform* transform;

        Text3dVisual() = default;

        Text3dVisual(
                Transform* transform,
                DrawableElements* drawable,
                const glm::vec3& color = { 0, 1, 0 },
                const float thickness = 1.0005f
        ) {
            this->transform = transform;
            this->drawable = drawable;
        }
    };

    struct GABRIEL_API PolygonVisualRenderer final {

        static PolygonFace polygonFace;
        static PolygonType polygonType;

        PolygonVisualRenderer();
        ~PolygonVisualRenderer();

        void begin();
        void end();

        void render(Drawable2dVisual& drawableVisual);
        void render(Drawable3dVisual& drawableVisual);

        void render(Text2dVisual& textVisual);
        void render(Text3dVisual& textVisual);

    private:
        Shader mShader;
        Shader mText2dShader;
        Shader mText3dShader;
    };

}