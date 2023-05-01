#pragma once

#include <image.h>
#include <rect.h>
#include <file_utils.h>

#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace gl {

    struct CharVertices : RectVertices<Vertex2dUV> {

        CharVertices() = default;

        CharVertices(
                const glm::vec2& pos0, const glm::vec2& uv0,
                const glm::vec2& pos1, const glm::vec2& uv1,
                const glm::vec2& pos2, const glm::vec2& uv2,
                const glm::vec2& pos3, const glm::vec2& uv3
        ) {
            v0.pos = pos0;
            v1.pos = pos1;
            v2.pos = pos2;
            v3.pos = pos3;

            v0.uv = uv0;
            v1.uv = uv1;
            v2.uv = uv2;
            v3.uv = uv3;
        }

    };

    struct Character {
        CharVertices vertices;
        glm::vec2 size = {};
        glm::vec2 bearing = {};
        float advance = 0;

        Character() = default;

        Character(const glm::vec2& size, const glm::vec2& bearing, float advance)
        : size(size), bearing(bearing), advance(advance) {}

        Character(const Character& character) = default;
        Character& operator =(const Character& character) = default;
    };

    typedef u32 font_id;

    struct Font final {
        font_id id;
        std::map<u8, Character> alphabet;
        Bitmap bitmap;
        ImageBuffer buffer;
        FT_Face face;
        u32 size;

        void init();
        void free();

        void set_size(u32 font_size);

        bool save_bmp(const char* filepath) const;
        bool save_widths(const char* filepath) const;

    private:
        int* widths = null;
        int width_size = 128;
    };

    struct FontAtlas final {
        static std::map<font_id, Font> fonts;

        static bool init();
        static void free();

        static Font* load(const char* filepath, u32 font_size);

    private:
        static bool load_face(const char* filepath, Font& font);

    private:
        static FT_Library library;
    };

}