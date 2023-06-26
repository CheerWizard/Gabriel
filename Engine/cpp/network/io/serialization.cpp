#include <network/io/serialization.h>

namespace gl {

    NetworkStream::NetworkStream(
            std::vector<std::string>& headers,
            BinaryStream& body
    ) {
        for (auto& header : headers) {
            mStream.addString(header);
        }
        mStream.add(body);
    }

    NetworkStream::~NetworkStream() {
        clear();
    }

    void NetworkStream::clear() {
        mStream.clear();
    }

}