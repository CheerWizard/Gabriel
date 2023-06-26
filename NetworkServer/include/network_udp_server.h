#pragma once

#include <network/udp/udp_server.h>

namespace gl {

    struct NetworkUDPServer : UDPServer {

        NetworkUDPServer() : UDPServer() {}

        NetworkUDPServer(const Address& address) : UDPServer(address) {}

    };

}