#include <text/font.h>

namespace gl {

    void Font::init() {
        buffer.init();
//        buffer.set_byte_alignment(1);

        bitmap.internalFormat = GL_RGB;
        bitmap.pixelFormat = GL_RGB;
        bitmap.pixelType = PixelType::U8;

        ImageParams params;
        params.s = GL_CLAMP_TO_EDGE;
        params.t = GL_CLAMP_TO_EDGE;
        params.r = GL_CLAMP_TO_EDGE;
        params.minFilter = GL_LINEAR;
        params.magFilter = GL_LINEAR;

        buffer.load(bitmap, params);

        id = buffer.id;
    }

    void Font::free() {
        bitmap.free();
        buffer.free();
        FT_Done_Face(face);
        delete[] mWidths;
    }

    bool FontAtlas::loadFace(const char* filepath, Font& font) {
        FT_Error error = FT_New_Face(sLib, filepath, 0, &font.face);
        if (error == FT_Err_Unknown_File_Format) {
            error("Failed to load font {0}. Unknown file format", filepath);
            return false;
        }
        else if (error) {
            error("Failed to load font {0}. Error: {1}", filepath, error);
            return false;
        }
        return true;
    }

    void Font::setSize(u32 font_size) {
        this->size = font_size;

        FT_UInt glyph_index;

        FT_Set_Pixel_Sizes(face, 0, font_size);

        bitmap.free();
        bitmap.width = (font_size + 2) * 16;
        bitmap.height = (font_size + 2) * 8;
        bitmap.channels = 4;
        bitmap.init();

        delete[] mWidths;
        mWidths = new int[mWidthSize];
        u8* pixels = (u8*) bitmap.pixels;

        // we need to find the character that goes below the baseline by the biggest value
        int max_under_baseline = 0;
        for (int i = 32; i < 127; ++i) {
            // get the glyph index from character code
            glyph_index = FT_Get_Char_Index(face, i);

            // load the glyph image into the slot
            FT_Error error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
            if (error) {
                error("Failed to load a glyph {0}. Error: {1}", glyph_index, error);
                continue;
            }

            // get the glyph metrics
            const FT_Glyph_Metrics& glyphMetrics = face->glyph->metrics;

            // find the character that reaches below the baseline by the biggest value
            int glyphHang = (glyphMetrics.horiBearingY - glyphMetrics.height) / 64;
            if (glyphHang < max_under_baseline) {
                max_under_baseline = glyphHang;
            }
        }

        // draw all characters
        for (int i = 0; i < 128; ++i) {
            // get the glyph index from character code
            glyph_index = FT_Get_Char_Index(face, i);

            // load the glyph image into the slot
            FT_Error error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
            if (error) {
                error("Failed to load a glyph {0}. Error: {1}", glyph_index, error);
                continue;
            }

            // convert to an anti-aliased bitmap
            error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
            if (error) {
                error("Failed to render a glyph {0}. Error: {1}", glyph_index, error);
            }

            // save the character width
            auto w = face->glyph->metrics.width / 64;
            auto h = face->glyph->metrics.height / 64;
            mWidths[i] = w;
            // find the tile position where we have to draw the character
            int x = (i % 16) * (font_size + 2);
            int y = (i / 16) * (font_size + 2);
            x += 1; // 1 pixel padding from the left side of the tile
            y += (font_size + 2) - face->glyph->bitmap_top + max_under_baseline - 1;
            // store metrics data in character
            glm::fvec2 image_size = { (float) bitmap.width, (float) bitmap.height };
            glm::fvec2 uv_normalizer = image_size * 2.0f;

            Character character;

            character.vertices.v0.uv = (glm::fvec2(x, y) * 2.0f - 1.0f) / uv_normalizer;
            character.vertices.v1.uv = (glm::fvec2(x + w, y) * 2.0f - 1.0f) / uv_normalizer;
            character.vertices.v2.uv = (glm::fvec2(x + w, y + h) * 2.0f - 1.0f) / uv_normalizer;
            character.vertices.v3.uv = (glm::fvec2(x, y + h) * 2.0f - 1.0f) / uv_normalizer;

            character.size = glm::fvec2(w, h) / image_size;
            character.bearing = glm::fvec2(face->glyph->bitmap_left, face->glyph->bitmap_top) / image_size;
            character.advance = (float) face->glyph->advance.x / (float) bitmap.width;
            // store character in alphabet table
            alphabet[i] = character;
            // store character in bitmap
            const FT_Bitmap& ft_bitmap = face->glyph->bitmap;
            for (int xx = 0; xx < ft_bitmap.width; ++xx) {
                for (int yy = 0; yy < ft_bitmap.rows; ++yy) {
                    u8 r = ft_bitmap.buffer[(yy * (ft_bitmap.width) + xx)];
                    int z = (y + yy) * bitmap.width * 4 + (x + xx) * 4;
                    pixels[z] = r;
                    pixels[z + 1] = r;
                    pixels[z + 2] = r;
                    pixels[z + 3] = 255;
                }
            }
        }
        // transform pixels into bitmap
        bitmap.initBmp();
    }

    void Font::saveBmp(const char* filepath) const {
        ImageWriter::write(filepath, bitmap);
    }

    void Font::saveWidths(const char* filepath) const {
        FileWriter::write(filepath, mWidths, mWidthSize);
    }

    FT_Library FontAtlas::sLib;
    std::map<FontId, Font> FontAtlas::fonts;
    std::map<FontId, std::string> FontAtlas::fontPaths;

    bool FontAtlas::init() {
        FT_Error error = FT_Init_FreeType(&sLib);
        if (error != FT_Err_Ok) {
            error("Failed to init FreeType library. Error: {0}", error);
            return false;
        }
        return true;
    }

    void FontAtlas::free() {
        for (auto& font : fonts) {
            font.second.free();
        }
        FT_Done_FreeType(sLib);
    }

    Font* FontAtlas::load(const char* filepath, u32 fontSize) {
        Font newFont;

        if (!loadFace(filepath, newFont))
            return null;

        newFont.setSize(fontSize);

        newFont.init();

        fonts[newFont.id] = newFont;
        fontPaths[newFont.id] = std::string(filepath);

        return &fonts[newFont.id];
    }

    void FontAtlas::remove(FontId fontId) {
        fonts[fontId].free();
        fonts[fontId] = {};
    }
}