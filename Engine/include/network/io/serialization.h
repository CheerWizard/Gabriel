#pragma once

#include <io/serialization.h>

namespace gl {

    struct GABRIEL_API NetworkStream final {

        NetworkStream() = default;
        NetworkStream(std::vector<std::string>& headers, BinaryStream& body);
        ~NetworkStream();

        inline char* data() {
            return reinterpret_cast<char*>(mStream.data());
        }

        inline const char* data() const {
            return reinterpret_cast<const char*>(mStream.data());
        }

        inline size_t size() const {
            return mStream.size();
        }

        void clear();

    private:
        BinaryStream mStream;
    };

}