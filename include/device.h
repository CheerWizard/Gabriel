#pragma once

namespace gl {

    struct device final {
        static int max_attrs_allowed;
        static float max_anisotropy_samples;
    };

    void init(int viewport_width, int viewport_height);

}