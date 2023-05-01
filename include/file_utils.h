#pragma once

#include <fstream>
#include <string>

namespace io {

    struct FileReader final {
        static std::string read(const char* filepath);
    };

    struct FileWriter final {
        static bool write(const char* filepath, const int* data, size_t size);
    };

}
