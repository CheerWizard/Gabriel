#include <api/commands.h>

namespace gl {

    void clearDisplay(float r, float g, float b, float a, int bitmask) {
        glClearColor(r, g, b, a);
        glClear(bitmask);
    }

    void polygonFill() {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void polygonLine() {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    void polygonPoint() {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }

}