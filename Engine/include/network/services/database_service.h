#pragma once

#include <network/tcp/tcp_client.h>

namespace gl {

    struct GABRIEL_API DatabaseService final {

        DatabaseService(TCPClient* client);
        ~DatabaseService();

        void save(BinaryStream& stream);
        void load(BinaryStream& stream);

    private:
        TCPClient* mClient = null;
    };

}