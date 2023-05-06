#include <io/writers.h>

namespace gl {

    bool FileWriter::write(const char* filepath, const int *data, size_t size) {
        std::ofstream ofs;
        ofs.open(filepath);

        if (ofs.is_open()) {
            for (int i = 0; i < size; ++i) {
                ofs << data[i] << std::endl;
            }
            ofs.close();
        } else {
            error("Failed to write to " << filepath << " with data size " << size)
            return false;
        }

        return true;
    }

}