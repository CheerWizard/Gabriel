#pragma once

#include <api/viewport.h>

namespace gl {

    struct Device final {
        int MAX_ATTRS_ALLOWED;
        float MAX_ANISOTROPY_SAMPLES;
        int MAX_WORKGROUP_COUNT[3];
        int MAX_WORKGROUP_SIZE[3];
        int MAX_WORKGROUP_INVOCATIONS;
        int MAX_SHADER_STORAGE_BUFFER_BINDINGS;
        int MAX_SHADER_STORAGE_BLOCK_SIZE;
        int MAX_VERTEX_SHADER_STORAGE_BLOCKS;
        int MAX_FRAGMENT_SHADER_STORAGE_BLOCKS;
        int MAX_GEOMETRY_SHADER_STORAGE_BLOCKS;
        int MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS;
        int MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS;
        int MAX_COMPUTE_SHADER_STORAGE_BLOCKS;
        int MAX_COMBINED_SHADER_STORAGE_BLOCKS;
        int MAX_SAMPLES;

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