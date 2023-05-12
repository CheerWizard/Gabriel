#pragma once

#include <api/image.h>

#include <geometry/rect.h>

#include <io/readers.h>
#include <io/writers.h>

#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H"freetype/freetype.h"

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

    typedef u32 FontId;

    struct Font final {
        FontId id;
        std::map<u8, Character> alphabet;
        Bitmap bitmap;
        ImageBuffer buffer;
        FT_Face face;
        u32 size;

        void init();
        void free();

        void setSize(u32 fontSize);

        bool saveBmp(const char* filepath) const;
        bool saveWidths(const char* filepath) const;

    private:
        int* mWidths = null;
        int mWidthSize = 128;
    };

    struct FontAtlas final {
        static std::map<FontId, Font> fonts;
        static std::map<FontId, std::string> fontPaths;

        static bool init();
        static void free();

        static Font* load(const char* filepath, u32 fontSize);
        static void remove(FontId fontId);

    private:
        static bool loadFace(const char* filepath, Font& font);

    private:
        static FT_Library sLib;
    };

}