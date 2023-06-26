#pragma once

namespace gl {

    struct GABRIEL_API FileReader final {
        static std::string read(const char* filepath);
    };

}
