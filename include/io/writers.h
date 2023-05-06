#pragma once

#include <core/common.h>

#include <fstream>
#include <string>

namespace gl {

    struct FileWriter final {
        static bool write(const char* filepath, const int* data, size_t size);
    };

}
