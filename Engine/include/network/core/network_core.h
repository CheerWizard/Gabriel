#pragma once

#ifdef WINDOWS
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
typedef unsigned __int64 SOCKET;
#endif

#ifdef LINUX
typedef s32 SOCKET;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#endif

#define localhost "127.0.0.1"

namespace gl {

    struct GABRIEL_API NetworkCore final {
        static void init();
        static void free();
    };

}