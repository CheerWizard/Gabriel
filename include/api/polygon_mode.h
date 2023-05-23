#pragma once

#include <glad/glad.h>

namespace gl {

    enum PolygonType : u32 {
        FILL = GL_FILL,
        POINT = GL_POINT,
        LINE = GL_LINE,
    };

    enum PolygonFace : u32 {
        FRONT = GL_FRONT,
        BACK = GL_BACK,
        FRONT_BACK = GL_FRONT_AND_BACK
    };

    struct PolygonMode final {
        static void enable();
        static void disable();
        static void set(const PolygonFace face, const PolygonType type);
    };

}