#pragma once

#include <primitives.h>

#define glCheckError() gl::check_err(__FILE__, __LINE__)

namespace gl {

    struct device final {
        static int max_attrs_allowed;
        static float max_anisotropy_samples;
    };

    void init(int viewport_width, int viewport_height);
    void resize(int w, int h);

    u32 check_err(const char* file, int line);

}