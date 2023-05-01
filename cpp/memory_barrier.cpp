#include <memory_barrier.h>

namespace gl {

    void memory_barrier(u32 bitfield) {
        glMemoryBarrier(bitfield);
    }

}