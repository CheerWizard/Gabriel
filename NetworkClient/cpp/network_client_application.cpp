#include "network/core/network_application.h"

namespace gl {

    struct NetworkClientApplication : NetworkApplication {

        NetworkClientApplication(const char* title, const char* ip, const u16 port)
        : NetworkApplication(title, ip, port) {}

        void run() override {
            mTcpClient = new TCPClient(m_ip, m_port, IPFamily::V4);
        }

    private:
        TCPClient* mTcpClient = null;
        UDPClient* mUdpClient = null;
    };

    NetworkApplication* createNetworkApplication(int argc, char** argv) {
        return new NetworkClientApplication("Client", localhost, 8080);
    }

}