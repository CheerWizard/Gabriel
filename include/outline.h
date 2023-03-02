#pragma once

#include <camera.h>
#include <transform.h>
#include <draw.h>

namespace gl {

    struct outline_t final {
        float thickness = 0.02f;
        glm::vec4 color = { 1, 1, 0, 1 };
    };

    outline_t outline_init();
    void outline_free();

    void outline_begin();
    void outline_end();

    void outline_draw(gl::transform& transform, const gl::drawable_elements& drawable, outline_t& outline);

}