#pragma once

#include <debugger.h>
#include <viewport.h>

namespace gl {

    struct Device final {
        static int MAX_ATTRS_ALLOWED;
        static float MAX_ANISOTROPY_SAMPLES;
        static int MAX_WORKGROUP_COUNT[3];
        static int MAX_WORKGROUP_SIZE[3];
        static int MAX_WORKGROUP_INVOCATIONS;

        static void init(int viewport_width, int viewport_height);
    };

}