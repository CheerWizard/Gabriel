#pragma once

#include <glad/glad.h>

namespace gl {

    void set_view_port(int x, int y, int w, int h);

    void clear_display(float r, float g, float b, float a, int bitmask = GL_COLOR_BUFFER_BIT);

    void polygon_fill();
    void polygon_line();
    void polygon_point();

}
