#pragma once

#include <network/tcp/tcp_server.h>

namespace gl {

    struct NetworkTCPServer : TCPServer {

        NetworkTCPServer() : TCPServer() {}

        NetworkTCPServer(const Address& address) : TCPServer(address) {}

    };

}