#include <api/viewport.h>

#include <glad/glad.h>

namespace gl {

    void Viewport::resize(int x, int y, int w, int h) {
        glViewport(x, y, w, h);
    }

}