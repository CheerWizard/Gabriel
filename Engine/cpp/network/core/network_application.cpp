#include "network/core/network_application.h"

namespace gl {

    NetworkApplication::NetworkApplication(const char* title, const char* ip, const u16 port)
    : m_title(title), m_ip(ip), m_port(port) {
        Logger::init(m_title, 32);
        NetworkCore::init();
    }

    NetworkApplication::~NetworkApplication() {
        NetworkCore::free();
        Logger::free();
    }

    void NetworkApplication::run() {}

}