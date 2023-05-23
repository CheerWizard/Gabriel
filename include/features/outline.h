#pragma once

#include <api/draw.h>

#include <features/transform.h>

namespace gl {

    component(Outline) {
        glm::vec4 color = { 1, 1, 0, 1 };
        float thickness = 0.02f;

        Outline(const float r = 1, const float g = 1, const float b = 0, const float a = 1.0f,
                const float thickness = 0.02f
        ) : color(r, g, b, a), thickness(thickness) {}
    };

    struct OutlineParams final {
        UniformV4F color = { "outline_color", { 1, 1, 0, 1 } };
        UniformF thickness = { "outline_thickness", 0.02f };
    };

    struct OutlineShader : Shader {
        OutlineParams params;

        void init();
        void update(Outline& outline);
    };

    struct OutlineRenderer final {

        OutlineRenderer();
        ~OutlineRenderer();

        void bind();
        void unbind();
        void use();

        void render(Outline& outline, Transform& transform, DrawableElements& drawable);

    private:
        OutlineShader mShader;
    };

}