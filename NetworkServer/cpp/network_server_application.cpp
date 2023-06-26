#include <network_server_application.h>

namespace gl {

    NetworkApplication* createNetworkApplication(int argc, char** argv) {
        return new NetworkServerApplication("Server", localhost, 8080);
    }

    void NetworkServerApplication::run() {
        m_tcpServer = new TCPServer(m_ip, m_port, IPFamily::V4);
    }

}