#pragma once

#include <window.h>

#include <glm/glm.hpp>

namespace ui {

    void init(GLFWwindow *window);

    void free();

    void begin();

    void end();

    typedef void (*draw_window_items_fn)();

    void window(const char *title, draw_window_items_fn draw_items);

    void image(u32 id, u16 w, u16 h);

    void slider(const char *label, float *value, float min, float max, float step);

    void checkbox(const char *label, int *value);

    void theme_selector(const char *label);

    void color_picker(const char *label, glm::vec4 &color);

}