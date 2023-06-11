#pragma once

namespace gl {

    struct FileWriter final {
        static void write(const char* filepath, const int* data, size_t size);
        static void write(const char* filepath, const std::string& buffer);
    };

}
