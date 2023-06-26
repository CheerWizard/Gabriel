#include "network/core/network_core.h"

namespace gl {

#ifdef WINDOWS

    void NetworkCore::init() {
        WSAData wsaData;
        WORD version = MAKEWORD(2, 2);
        int wsInit = WSAStartup(version, &wsaData);

        if (wsInit != 0) {
            error("Unable to initialize Windows network core version: {0}", version);
        }

        info("Windows network core initialized!");
    }

    void NetworkCore::free() {
        WSACleanup();
        info("Window network core destroyed!");
    }

#elif LINUX

    void NetworkCore::init() {
        info("Linux network core initialized!");
    }

    void NetworkCore::free() {
        info("Linux network core destroyed!");
    }

#endif

}