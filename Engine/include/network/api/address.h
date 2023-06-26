#pragma once

#include <network/core/network_logger.h>

namespace gl {

    enum IPFamily : u16 {
        None,
        V4 = AF_INET,
        V6 = AF_INET6
    };

    struct GABRIEL_API SocketAddress final {
        sockaddr* value = null;
        int size = 0;

        SocketAddress() = default;
        SocketAddress(sockaddr* value, int size) : value(value), size(size) {}

        inline bool valid() const {
            return value != null;
        }
    };

    struct GABRIEL_API Address final {
        IPFamily family = IPFamily::None;
        std::string hostname = "";
        std::string string = "";
        std::vector<u8> bytes;
        u16 port = 0;
        u32 value = INADDR_NONE;

        Address() = default;
        Address(const char* ip, u16 port, IPFamily ipFamily);
        Address(const SocketAddress& socketAddress);

        SocketAddress getSocketAddress();

    private:
        void initV4(const char* ip);
        void initV6(const char* ip);

        SocketAddress getSockAddrV4();
        SocketAddress getSockAddrV6();

    private:
        SocketAddress mSockAddr;
        sockaddr_in mAddrV4;
        sockaddr_in6 mAddrV6;
    };

}