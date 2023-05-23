#include <api/polygon_mode.h>

namespace gl {

    void PolygonMode::enable() {
        glEnable(GL_POLYGON_MODE);
    }

    void PolygonMode::disable() {
        glDisable(GL_POLYGON_MODE);
    }

    void PolygonMode::set(const PolygonFace face, const PolygonType type) {
        glPolygonMode(face, type);
    }

}