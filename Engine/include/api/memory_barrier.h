#pragma once

#include <glad/glad.h>

namespace gl {

    enum MemoryBarrierBits : u32 {
        ALL = GL_ALL_BARRIER_BITS,
        SHADER_IMAGE_ACCESS = GL_SHADER_IMAGE_ACCESS_BARRIER_BIT
    };

    GABRIEL_API void memoryBarrier(u32 bitfield);

}