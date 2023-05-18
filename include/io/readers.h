#pragma once

namespace gl {

    struct FileReader final {
        static std::string read(const char* filepath);
    };

}
