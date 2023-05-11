#pragma once

#include <core/common.h>

namespace gl {

    struct StorageBuffer {
        u32 id;

        void init(u32 binding, long long size);
        void free();
        void bind() const;

        void update(long long offset, long long size, void* data) const;
    };

}