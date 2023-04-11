#pragma once

#include <fstream>
#include <string>

namespace io {

    struct FileReader final {
        static std::string read(const char* filepath);
    };

}
