#pragma once

#include <core/common.h>

#include <fstream>
#include <string>

namespace gl {

    struct FileReader final {
        static std::string read(const char* filepath);
    };

}
