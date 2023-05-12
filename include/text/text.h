#pragma once

#include <text/font.h>

#include <features/transform.h>

#include <functional>

namespace gl {

    struct Style final {
        Font* font = null;
        glm::vec4 color = { 1, 1, 1, 1 };
        glm::vec2 padding = { 0, 0 };

        Style() = default;
        Style(Font* font) : font(font) {}
        Style(Font* font, const glm::vec4& color)
        : font(font), color(color) {}
        Style(Font* font, const glm::vec4& color, const glm::vec2& padding)
        : font(font), color(color), padding(padding) {}
    };

    struct Text {
        Style style;
        std::string buffer;

        Text() {
            init();
        }

        Text(const char* buffer) : buffer(buffer) {
            init();
        }

        Text(const std::string& buffer) : buffer(buffer) {
            init();
        }

        Text(const Style& style, const std::string& buffer) : style(style), buffer(buffer) {
            init();
            update();
        }

        Text(const Style& style, const char* buffer) : style(style), buffer(buffer) {
            init();
            update();
        }

        void init();
        void free();

        void update();

        void draw();

    private:
        DrawableElements mDrawable;
        std::vector<CharVertices> mVertices;
        std::vector<u32> mIndices;
    };

    component(Text2d), Text {
        Transform2d transform;

        Text2d() : Text() {}

        ~Text2d() {
            free();
        }

        Text2d(const std::string& buffer) : Text(buffer) {}
        Text2d(const char* buffer) : Text(buffer) {}
        Text2d(const Style& style, const std::string& buffer) : Text(style, buffer) {}
        Text2d(const Style& style, const char* buffer) : Text(style, buffer) {}
    };

    component(Text3d), Text {
        Transform transform;

        Text3d() : Text() {}

        ~Text3d() {
            free();
        }

        Text3d(const std::string& buffer) : Text(buffer) {}
        Text3d(const char* buffer) : Text(buffer) {}
        Text3d(const Style& style, const std::string& buffer) : Text(style, buffer) {}
        Text3d(const Style& style, const char* buffer) : Text(style, buffer) {}
    };

    struct TextShader : Shader {
        UniformV4F textColor = { "textColor", { 1, 1, 1, 1 } };

        void update(Style& style);
    };

    struct Text2dRenderer final {

        Text2dRenderer();
        ~Text2dRenderer();

        void begin();
        void render(Text2d& text);

    private:
        TextShader mShader;
    };

    struct Text3dRenderer final {

        Text3dRenderer();
        ~Text3dRenderer();

        void begin();
        void render(Text3d& text);

    private:
        TextShader mShader;
    };

}