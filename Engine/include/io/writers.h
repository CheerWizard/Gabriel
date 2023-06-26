#pragma once

namespace gl {

    struct GABRIEL_API FileWriter final {
        static void write(const char* filepath, const int* data, size_t size);
        static void write(const char* filepath, const std::string& buffer);
    };

}
