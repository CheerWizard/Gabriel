#include <network/api/address.h>
#include <inaddr.h>

namespace gl {

    Address::Address(const char* ip, u16 port, IPFamily ipFamily) : port(port), family(ipFamily) {
        switch (family) {

            case IPFamily::V4:
                initV4(ip);
                break;

            case IPFamily::V6:
                initV6(ip);
                break;

            default:
                error("Address IP family not supported!");
                break;

        }
    }

    Address::Address(const SocketAddress& socketAddress) : mSockAddr(socketAddress) {
        family = static_cast<IPFamily>(socketAddress.value->sa_family);

        switch (family) {

            case IPFamily::V4:
                mAddrV4 = *reinterpret_cast<sockaddr_in*>(socketAddress.value);

                port = ntohs(mAddrV4.sin_port);

                value = mAddrV4.sin_addr.S_un.S_addr;

                bytes.resize(sizeof(value));
                memcpy(bytes.data(), &value, sizeof(value));

                string.resize(16);
                inet_ntop(family, &mAddrV4.sin_addr, string.data(), 16);

                hostname = string;

                break;

            case IPFamily::V6:
                mAddrV6 = *reinterpret_cast<sockaddr_in6*>(socketAddress.value);

                port = ntohs(mAddrV6.sin6_port);

                memcpy(&value, &mAddrV6.sin6_addr.u, sizeof(mAddrV6.sin6_addr.u));

                bytes.resize(sizeof(value));
                memcpy(bytes.data(), &value, sizeof(value));

                string.resize(46);
                inet_ntop(family, &mAddrV6.sin6_addr, string.data(), 46);

                hostname = string;

                break;

            default:
                error("Address IP family not supported!");
                break;
        }
    }

    void Address::initV4(const char* ip) {
        in_addr addr;
        int result = inet_pton(family, ip, &addr);

        if (result == 1) {
            value = addr.S_un.S_addr;
            if (value != INADDR_NONE) {
                string = ip;
                hostname = ip;
                bytes.resize(sizeof(value));
                memcpy(bytes.data(), &value, sizeof(value));
            }
        }

        // hostname -> IPv4
        addrinfo hints = {};
        hints.ai_family = family;
        addrinfo* hostInfo = null;
        result = getaddrinfo(ip, NULL, &hints, &hostInfo);
        if (result == 0) {
            sockaddr_in* hostAddr = reinterpret_cast<sockaddr_in*>(hostInfo->ai_addr);

            // IPv4 -> string presentation requires 16 bytes
            string.resize(16);
            inet_ntop(family, &hostAddr->sin_addr, string.data(), 16);

            hostname = ip;
            ULONG ipLong = hostAddr->sin_addr.S_un.S_addr;
            bytes.resize(sizeof(ipLong));
            memcpy(bytes.data(), &ipLong, sizeof(ipLong));

            freeaddrinfo(hostInfo);
        }
    }

    void Address::initV6(const char* ip) {
        in_addr6 addr;
        int result = inet_pton(family, ip, &addr);

        if (result == 1) {
            memcpy(&value, &addr.u, sizeof(addr.u));
            string = ip;
            hostname = ip;
            bytes.resize(sizeof(value));
            memcpy(bytes.data(), &value, sizeof(value));
        }

        // hostname -> IPv6
        addrinfo hints = {};
        hints.ai_family = family;
        addrinfo* hostInfo = null;
        result = getaddrinfo(ip, NULL, &hints, &hostInfo);
        if (result == 0) {
            sockaddr_in6* hostAddr = reinterpret_cast<sockaddr_in6*>(hostInfo->ai_addr);

            // IPv6 -> string presentation requires 46 bytes
            string.resize(46);
            inet_ntop(family, &hostAddr->sin6_addr, string.data(), 46);

            hostname = ip;
            memcpy(&value, &hostAddr->sin6_addr.u, sizeof(hostAddr->sin6_addr.u));
            bytes.resize(sizeof(value));
            memcpy(bytes.data(), &value, sizeof(value));

            freeaddrinfo(hostInfo);
        }
    }

    SocketAddress Address::getSocketAddress() {
        switch (family) {

            case IPFamily::V4:
                mSockAddr = getSockAddrV4();
                break;

            case IPFamily::V6:
                mSockAddr = getSockAddrV6();
                break;

            default:
                error("Socket address IP family not supported!");
                mSockAddr = SocketAddress();
                break;

        }

        return mSockAddr;
    }

    SocketAddress Address::getSockAddrV4() {
        memcpy(&mAddrV4.sin_addr, bytes.data(), sizeof(value));
        mAddrV4.sin_family = family;
        mAddrV4.sin_port = htons(port);
        return { reinterpret_cast<sockaddr*>(&mAddrV4), sizeof(sockaddr_in) };
    }

    SocketAddress Address::getSockAddrV6() {
        memcpy(&mAddrV6.sin6_addr, bytes.data(), sizeof(value));
        mAddrV6.sin6_family = family;
        mAddrV6.sin6_port = htons(port);
        return { reinterpret_cast<sockaddr*>(&mAddrV6), sizeof(sockaddr_in6) };
    }

}