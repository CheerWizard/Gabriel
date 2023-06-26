#include <api/memory_barrier.h>

namespace gl {

    void memoryBarrier(u32 bitfield) {
        glMemoryBarrier(bitfield);
    }

}