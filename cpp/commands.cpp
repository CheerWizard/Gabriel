#include <commands.h>

namespace gl {

    void clear_display(float r, float g, float b, float a, int bitmask) {
        glClearColor(r, g, b, a);
        glClear(bitmask);
    }

    void polygon_fill() {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void polygon_line() {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    void polygon_point() {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }

}