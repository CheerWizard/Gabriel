#include <font.h>

namespace gl {

    void Font::init() {
        buffer.init();
//        buffer.set_byte_alignment(1);

        bitmap.internal_format = GL_RGB;
        bitmap.pixel_format = GL_RGB;
        bitmap.pixel_type = PixelType::U8;

        ImageParams params;
        params.s = GL_CLAMP_TO_EDGE;
        params.t = GL_CLAMP_TO_EDGE;
        params.r = GL_CLAMP_TO_EDGE;
        params.min_filter = GL_LINEAR;
        params.mag_filter = GL_LINEAR;

        buffer.load(bitmap, params);

        id = buffer.id;
    }

    void Font::free() {
        bitmap.free();
        buffer.free();
        FT_Done_Face(face);
        delete[] widths;
    }

    bool FontAtlas::load_face(const char* filepath, Font& font) {
        FT_Error error = FT_New_Face(library, filepath, 0, &font.face);
        if (error == FT_Err_Unknown_File_Format) {
            print_err("Failed to load font " << filepath << "\nUnknown file format");
            return false;
        }
        else if (error) {
            print_err("Failed to load font " << filepath << "\nError: " << error);
            return false;
        }
        return true;
    }

    void Font::set_size(u32 size) {
        this->size = size;

        FT_UInt glyph_index;

        FT_Set_Pixel_Sizes(face, 0, size);

        bitmap.free();
        bitmap.width = (size + 2) * 16;
        bitmap.height = (size + 2) * 8;
        bitmap.channels = 4;
        bitmap.init();

        delete[] widths;
        widths = new int[width_size];
        u8* pixels = (u8*) bitmap.pixels;

        // we need to find the character that goes below the baseline by the biggest value
        int max_under_baseline = 0;
        for (int i = 32; i < 127; ++i) {
            // get the glyph index from character code
            glyph_index = FT_Get_Char_Index(face, i);

            // load the glyph image into the slot
            FT_Error error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
            if (error) {
                print_err("Failed to load a glyph " << glyph_index << "\nError: " << error);
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
                print_err("Failed to load a glyph " << glyph_index << "\nError: " << error);
                continue;
            }

            // convert to an anti-aliased bitmap
            error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
            if (error) {
                print_err("Failed to render a glyph " << glyph_index << "\nError: " << error);
            }

            // save the character width
            auto w = face->glyph->metrics.width / 64;
            auto h = face->glyph->metrics.height / 64;
            widths[i] = w;
            // find the tile position where we have to draw the character
            int x = (i % 16) * (size + 2);
            int y = (i / 16) * (size + 2);
            x += 1; // 1 pixel padding from the left side of the tile
            y += (size + 2) - face->glyph->bitmap_top + max_under_baseline - 1;
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
        bitmap.init_bmp();
    }

    bool Font::save_bmp(const char* filepath) const {
        return ImageWriter::write(filepath, bitmap);
    }

    bool Font::save_widths(const char* filepath) const {
        return io::FileWriter::write(filepath, widths, width_size);
    }

    FT_Library FontAtlas::library;
    std::map<font_id, Font> FontAtlas::fonts;

    bool FontAtlas::init() {
        FT_Error error = FT_Init_FreeType(&library);
        if (error != FT_Err_Ok) {
            print_err("Failed to init FreeType library\nError: " << error);
            return false;
        }
        return true;
    }

    void FontAtlas::free() {
        for (auto& font : fonts) {
            font.second.free();
        }
        FT_Done_FreeType(library);
    }

    Font* FontAtlas::load(const char* filepath, u32 font_size) {
        Font new_font;

        if (!load_face(filepath, new_font))
            return null;

        new_font.set_size(font_size);

        new_font.init();

        fonts[new_font.id] = new_font;

        return &fonts[new_font.id];
    }
}