#include <geometry.h>

namespace gl {

    void Indices::init(int count) {
        this->count = count;
        indices = (u32*) std::malloc(size());
    }

    void Indices::free() {
        std::free(indices);
        count = 0;
    }

    void Indices::copy_from(Indices* src) {
        free();
        init(src->count);
        memcpy(indices, src->indices, src->size());
    }

}