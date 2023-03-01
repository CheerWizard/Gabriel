#include <ui.h>
#include <commands.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT

#include <nuklear.h>
#include <nuklear_glfw_gl3.h>

#define NK_MAX_VERTEX_BUFFER 512 * 1024
#define NK_MAX_ELEMENT_BUFFER 128 * 1024

#include <unordered_map>
#include <string>

namespace ui {

    static nk_glfw glfw = { null };
    static nk_context* nk;

    enum theme { THEME_WHITE, THEME_RED, THEME_BLUE, THEME_DARK };

    static void set_style(nk_context *ctx, ui::theme theme)
    {
        nk_color table[NK_COLOR_COUNT];
        if (theme == THEME_WHITE) {
            table[NK_COLOR_TEXT] = nk_rgba(70, 70, 70, 255);
            table[NK_COLOR_WINDOW] = nk_rgba(175, 175, 175, 255);
            table[NK_COLOR_HEADER] = nk_rgba(175, 175, 175, 255);
            table[NK_COLOR_BORDER] = nk_rgba(0, 0, 0, 255);
            table[NK_COLOR_BUTTON] = nk_rgba(185, 185, 185, 255);
            table[NK_COLOR_BUTTON_HOVER] = nk_rgba(170, 170, 170, 255);
            table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(160, 160, 160, 255);
            table[NK_COLOR_TOGGLE] = nk_rgba(150, 150, 150, 255);
            table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(120, 120, 120, 255);
            table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(175, 175, 175, 255);
            table[NK_COLOR_SELECT] = nk_rgba(190, 190, 190, 255);
            table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(175, 175, 175, 255);
            table[NK_COLOR_SLIDER] = nk_rgba(190, 190, 190, 255);
            table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(80, 80, 80, 255);
            table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(70, 70, 70, 255);
            table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(60, 60, 60, 255);
            table[NK_COLOR_PROPERTY] = nk_rgba(175, 175, 175, 255);
            table[NK_COLOR_EDIT] = nk_rgba(150, 150, 150, 255);
            table[NK_COLOR_EDIT_CURSOR] = nk_rgba(0, 0, 0, 255);
            table[NK_COLOR_COMBO] = nk_rgba(175, 175, 175, 255);
            table[NK_COLOR_CHART] = nk_rgba(160, 160, 160, 255);
            table[NK_COLOR_CHART_COLOR] = nk_rgba(45, 45, 45, 255);
            table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba( 255, 0, 0, 255);
            table[NK_COLOR_SCROLLBAR] = nk_rgba(180, 180, 180, 255);
            table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(140, 140, 140, 255);
            table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(150, 150, 150, 255);
            table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(160, 160, 160, 255);
            table[NK_COLOR_TAB_HEADER] = nk_rgba(180, 180, 180, 255);
            nk_style_from_table(ctx, table);
        } else if (theme == THEME_RED) {
            table[NK_COLOR_TEXT] = nk_rgba(190, 190, 190, 255);
            table[NK_COLOR_WINDOW] = nk_rgba(30, 33, 40, 215);
            table[NK_COLOR_HEADER] = nk_rgba(181, 45, 69, 220);
            table[NK_COLOR_BORDER] = nk_rgba(51, 55, 67, 255);
            table[NK_COLOR_BUTTON] = nk_rgba(181, 45, 69, 255);
            table[NK_COLOR_BUTTON_HOVER] = nk_rgba(190, 50, 70, 255);
            table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(195, 55, 75, 255);
            table[NK_COLOR_TOGGLE] = nk_rgba(51, 55, 67, 255);
            table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(45, 60, 60, 255);
            table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(181, 45, 69, 255);
            table[NK_COLOR_SELECT] = nk_rgba(51, 55, 67, 255);
            table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(181, 45, 69, 255);
            table[NK_COLOR_SLIDER] = nk_rgba(51, 55, 67, 255);
            table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(181, 45, 69, 255);
            table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(186, 50, 74, 255);
            table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(191, 55, 79, 255);
            table[NK_COLOR_PROPERTY] = nk_rgba(51, 55, 67, 255);
            table[NK_COLOR_EDIT] = nk_rgba(51, 55, 67, 225);
            table[NK_COLOR_EDIT_CURSOR] = nk_rgba(190, 190, 190, 255);
            table[NK_COLOR_COMBO] = nk_rgba(51, 55, 67, 255);
            table[NK_COLOR_CHART] = nk_rgba(51, 55, 67, 255);
            table[NK_COLOR_CHART_COLOR] = nk_rgba(170, 40, 60, 255);
            table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba( 255, 0, 0, 255);
            table[NK_COLOR_SCROLLBAR] = nk_rgba(30, 33, 40, 255);
            table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(64, 84, 95, 255);
            table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(70, 90, 100, 255);
            table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(75, 95, 105, 255);
            table[NK_COLOR_TAB_HEADER] = nk_rgba(181, 45, 69, 220);
            nk_style_from_table(ctx, table);
        } else if (theme == THEME_BLUE) {
            table[NK_COLOR_TEXT] = nk_rgba(20, 20, 20, 255);
            table[NK_COLOR_WINDOW] = nk_rgba(202, 212, 214, 215);
            table[NK_COLOR_HEADER] = nk_rgba(137, 182, 224, 220);
            table[NK_COLOR_BORDER] = nk_rgba(140, 159, 173, 255);
            table[NK_COLOR_BUTTON] = nk_rgba(137, 182, 224, 255);
            table[NK_COLOR_BUTTON_HOVER] = nk_rgba(142, 187, 229, 255);
            table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(147, 192, 234, 255);
            table[NK_COLOR_TOGGLE] = nk_rgba(177, 210, 210, 255);
            table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(182, 215, 215, 255);
            table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(137, 182, 224, 255);
            table[NK_COLOR_SELECT] = nk_rgba(177, 210, 210, 255);
            table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(137, 182, 224, 255);
            table[NK_COLOR_SLIDER] = nk_rgba(177, 210, 210, 255);
            table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(137, 182, 224, 245);
            table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(142, 188, 229, 255);
            table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(147, 193, 234, 255);
            table[NK_COLOR_PROPERTY] = nk_rgba(210, 210, 210, 255);
            table[NK_COLOR_EDIT] = nk_rgba(210, 210, 210, 225);
            table[NK_COLOR_EDIT_CURSOR] = nk_rgba(20, 20, 20, 255);
            table[NK_COLOR_COMBO] = nk_rgba(210, 210, 210, 255);
            table[NK_COLOR_CHART] = nk_rgba(210, 210, 210, 255);
            table[NK_COLOR_CHART_COLOR] = nk_rgba(137, 182, 224, 255);
            table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba( 255, 0, 0, 255);
            table[NK_COLOR_SCROLLBAR] = nk_rgba(190, 200, 200, 255);
            table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(64, 84, 95, 255);
            table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(70, 90, 100, 255);
            table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(75, 95, 105, 255);
            table[NK_COLOR_TAB_HEADER] = nk_rgba(156, 193, 220, 255);
            nk_style_from_table(ctx, table);
        } else if (theme == THEME_DARK) {
            table[NK_COLOR_TEXT] = nk_rgba(210, 210, 210, 255);
            table[NK_COLOR_WINDOW] = nk_rgba(57, 67, 71, 215);
            table[NK_COLOR_HEADER] = nk_rgba(51, 51, 56, 220);
            table[NK_COLOR_BORDER] = nk_rgba(46, 46, 46, 255);
            table[NK_COLOR_BUTTON] = nk_rgba(48, 83, 111, 255);
            table[NK_COLOR_BUTTON_HOVER] = nk_rgba(58, 93, 121, 255);
            table[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(63, 98, 126, 255);
            table[NK_COLOR_TOGGLE] = nk_rgba(50, 58, 61, 255);
            table[NK_COLOR_TOGGLE_HOVER] = nk_rgba(45, 53, 56, 255);
            table[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(48, 83, 111, 255);
            table[NK_COLOR_SELECT] = nk_rgba(57, 67, 61, 255);
            table[NK_COLOR_SELECT_ACTIVE] = nk_rgba(48, 83, 111, 255);
            table[NK_COLOR_SLIDER] = nk_rgba(50, 58, 61, 255);
            table[NK_COLOR_SLIDER_CURSOR] = nk_rgba(48, 83, 111, 245);
            table[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
            table[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
            table[NK_COLOR_PROPERTY] = nk_rgba(50, 58, 61, 255);
            table[NK_COLOR_EDIT] = nk_rgba(50, 58, 61, 225);
            table[NK_COLOR_EDIT_CURSOR] = nk_rgba(210, 210, 210, 255);
            table[NK_COLOR_COMBO] = nk_rgba(50, 58, 61, 255);
            table[NK_COLOR_CHART] = nk_rgba(50, 58, 61, 255);
            table[NK_COLOR_CHART_COLOR] = nk_rgba(48, 83, 111, 255);
            table[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);
            table[NK_COLOR_SCROLLBAR] = nk_rgba(50, 58, 61, 255);
            table[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(48, 83, 111, 255);
            table[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(53, 88, 116, 255);
            table[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(58, 93, 121, 255);
            table[NK_COLOR_TAB_HEADER] = nk_rgba(48, 83, 111, 255);
            nk_style_from_table(ctx, table);
        } else {
            nk_style_default(ctx);
        }
    }

    void init(GLFWwindow* window) {
        nk = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);

        struct nk_font_atlas *atlas;
        nk_glfw3_font_stash_begin(&glfw, &atlas);
        struct nk_font* roboto = nk_font_atlas_add_from_file(atlas, "fonts/Roboto-Regular.ttf", 18, 0);
        nk_glfw3_font_stash_end(&glfw);
        nk_style_load_all_cursors(nk, atlas->cursors);
        nk_style_set_font(nk, &roboto->handle);

        set_style(nk, THEME_RED);
    }

    void free() {
        nk_glfw3_shutdown(&glfw);
    }

    void begin() {
        nk_glfw3_new_frame(&glfw);
    }

    void end() {
        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, NK_MAX_VERTEX_BUFFER, NK_MAX_ELEMENT_BUFFER);
    }

    static std::unordered_map<u32, struct nk_image> images;

    void window(const char* title, draw_window_items_fn draw_items) {
        if (nk_begin(nk, title, nk_rect(50, 50, 230, 250),
                     NK_WINDOW_BORDER|
                     NK_WINDOW_MOVABLE|
                     NK_WINDOW_SCALABLE|
                     NK_WINDOW_MINIMIZABLE|
                     NK_WINDOW_TITLE
        )) {
            draw_items();
        }
        nk_end(nk);
    }

    void image(u32 id, u16 w, u16 h) {
        if (images.find(id) == images.end()) {
            print_err("creating image " << id);
            images[id] = nk_image_id((int) id);
        }
        auto& img = images[id];
        nk_layout_row_static(nk, h, w, 1);
        nk_image(nk, img);
    }

    void slider(const char* label, float* value, float min, float max, float step) {
        nk_layout_row_dynamic(nk, 20, 5);
        nk_label(nk, label, NK_TEXT_LEFT);
        nk_label(nk, std::to_string(*value).c_str(), NK_TEXT_LEFT);
        nk_label(nk, std::to_string(min).c_str(), NK_TEXT_LEFT);
        nk_slider_float(nk, min, value, max, step);
        nk_label(nk, std::to_string(max).c_str(), NK_TEXT_LEFT);
    }

    void color_picker(const char* label, glm::vec4& color) {
        nk_layout_row_dynamic(nk, 20, 1);
        nk_label(nk, label, NK_TEXT_LEFT);
        nk_layout_row_dynamic(nk, 25, 1);
        nk_colorf c = { color.r, color.g, color.b };
        if (nk_combo_begin_color(nk, nk_rgb_cf(c), nk_vec2(nk_widget_width(nk),400))) {
            nk_layout_row_dynamic(nk, 120, 1);
            c = nk_color_picker(nk, c, NK_RGB);
            nk_layout_row_dynamic(nk, 25, 1);
            color.r = nk_propertyf(nk, "#R:", 0, color.r, 1.0f, 0.01f,0.005f);
            color.g = nk_propertyf(nk, "#G:", 0, color.g, 1.0f, 0.01f,0.005f);
            color.b = nk_propertyf(nk, "#B:", 0, color.b, 1.0f, 0.01f,0.005f);
            color.b = nk_propertyf(nk, "#A:", 0, color.a, 1.0f, 0.01f,0.005f);
            color = { c.r, c.g, c.b, c.a };
            nk_combo_end(nk);
        }
    }

    void checkbox(const char* label, int* value) {
        nk_layout_row_dynamic(nk, 20, 1);
        nk_checkbox_label(nk, label, value);
    }

    void theme_selector(const char* label) {
        nk_layout_row_dynamic(nk, 24, 5);

        nk_label(nk, label, NK_TEXT_LEFT);

        if (nk_button_color(nk, { 255, 255, 255, 255 })) {
            set_style(nk, THEME_WHITE);
        }
        if (nk_button_color(nk, { 0, 0, 0, 255 })) {
            set_style(nk, THEME_DARK);
        }
        if (nk_button_color(nk, { 255, 0, 0, 255 })) {
            set_style(nk, THEME_RED);
        }
        if (nk_button_color(nk, { 0, 0, 255, 255 })) {
            set_style(nk, THEME_BLUE);
        }
    }

}