#pragma once

#include <core/common.h>

#include <glad/glad.h>

namespace gl {

    enum MemoryBarrierBits : u8 {
        ALL = GL_ALL_BARRIER_BITS,
        SHADER_IMAGE_ACCESS = GL_SHADER_IMAGE_ACCESS_BARRIER_BIT
    };

    void memoryBarrier(u32 bitfield);

}