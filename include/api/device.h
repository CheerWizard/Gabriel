#pragma once

#include <api/debugger.h>
#include <api/viewport.h>

namespace gl {

    struct Device final {
        int MAX_ATTRS_ALLOWED;
        float MAX_ANISOTROPY_SAMPLES;
        int MAX_WORKGROUP_COUNT[3];
        int MAX_WORKGROUP_SIZE[3];
        int MAX_WORKGROUP_INVOCATIONS;

        Device(int width, int height);
        ~Device();

        static Device& get() {
            return *sInstance;
        }

        static void setPolygonOffset(float factor, float units);

    private:
        static Device* sInstance;
    };

}