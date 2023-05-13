#include <io/writers.h>

namespace gl {

    bool FileWriter::write(const char* filepath, const int* data, size_t size) {
        std::ofstream ofs;
        ofs.open(filepath);

        if (ofs.is_open()) {
            for (int i = 0; i < size; ++i)
                ofs << data[i] << "\n";
        } else {
            error("Failed to write to {0} with data size {1}", filepath, size);
            return false;
        }

        return true;
    }

    bool FileWriter::write(const char* filepath, const std::string &buffer) {
        std::ofstream ofs;
        ofs.open(filepath);

        if (ofs.is_open()) {
            ofs << buffer << "\n";
        } else {
            error("Failed to write to {0} with data size {1}", filepath, buffer.length());
            return false;
        }

        return true;
    }

}