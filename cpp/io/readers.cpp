#include <io/readers.h>

namespace gl {

    std::string FileReader::read(const char* filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            error("Failed to open file {0}", filepath);
            return "";
        }

        std::string buffer;
        std::string line;
        while (std::getline(file, line)) {
            buffer.append(line);
            buffer.append("\n");
            if (file.eof())
                break;
        }

        file.close();

        return buffer;
    }

}