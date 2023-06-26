#pragma once

#include "network/core/network_application.h"
#include <network_tcp_server.h>
#include <network_udp_server.h>

namespace gl {

    struct NetworkServerApplication : NetworkApplication {

        NetworkServerApplication(const char* title, const char* ip, const u16 port)
        : NetworkApplication(title, ip, port) {}

        void run() override;

    private:
        NetworkTCPServer* m_tcpServer = null;
        NetworkUDPServer* m_udpServer = null;
    };

}