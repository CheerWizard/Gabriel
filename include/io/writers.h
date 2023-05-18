#pragma once

namespace gl {

    struct FileWriter final {
        static bool write(const char* filepath, const int* data, size_t size);
        static bool write(const char* filepath, const std::string& buffer);
    };

}
