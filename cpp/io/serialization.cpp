#include <io/serialization.h>
#include <io/writers.h>
#include <io/readers.h>

namespace gl {

    BinaryStream::BinaryStream(size_t capacity) {
        mBuffer.reserve(capacity);
    }

    BinaryStream::BinaryStream(std::string& string) {
        addString(string);
    }

    BinaryStream::~BinaryStream() {
        mBuffer.clear();
        mCursor = 0;
    }

    void BinaryStream::addString(std::string& string) {
        size_t length = string.length();
        add(length);
        add(string.data(), length);
    }

    void BinaryStream::add(void* data, size_t newSize) {
        mBuffer.resize(mBuffer.size() + newSize);
        memcpy(mBuffer.data() + mCursor, data, newSize);
        mCursor += newSize;
    }

    void BinaryStream::getString(std::string& string) {
        size_t length = 0;
        get(length);
        string.resize(length);
        get(string.data(), length);
    }

    void BinaryStream::get(void* data, size_t size) {
        memcpy(data, mBuffer.data() + mCursor, size);
        mCursor += size;
    }

    void BinaryStream::write(const char* filepath) {
        std::ofstream file(filepath);

        if (!file.is_open()) {
            error("Failed to open file {0}", filepath);
            exception("BinaryStream write exception");
        }

        size_t size = mBuffer.size();
        file << size;
        file.write(reinterpret_cast<const char*>(mBuffer.data()), size);

        file.close();
    }

    void BinaryStream::read(const char* filepath) {
        std::ifstream file(filepath);

        if (!file.is_open()) {
            error("Failed to open file {0}", filepath);
            exception("BinaryStream read exception");
        }

        size_t fileSize = 0;
        file >> fileSize;
        mBuffer.resize(fileSize);
        file.read(reinterpret_cast<char*>(mBuffer.data()), fileSize);
        mCursor += fileSize;

        file.close();
    }

    void BinaryStream::seek(size_t index) {
        mCursor = index;
    }

}