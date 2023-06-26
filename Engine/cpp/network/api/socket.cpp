#include <network/api/socket.h>

namespace gl {

    Socket::Socket() = default;

    Socket::Socket(SOCKET handle) : mHandle(handle) {}

    Socket::Socket(SOCKET handle, const SocketAddress& socketAddress) : mHandle(handle), address(socketAddress) {}

    Socket::Socket(SocketType type, IPFamily ipFamily, const std::vector<SocketOption>& options) {
        address.family = ipFamily;

        open(type);

        if (!invalid()) {
            for (auto option : options) {
                setOption(option, TRUE);
            }
        }
    }

    Socket::~Socket() {
        close();
    }

    NetworkCode Socket::setOption(SocketOption socketOption, BOOL value) {
        int result = 0;
        const char* name = null;
        NetworkCode code = NetworkCode::NONE;

        switch (socketOption) {

            case SocketOption::TCP_NoDelay:
                name = "TCP_NoDelay";
                result = setsockopt(mHandle, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&value), sizeof(value));
                break;

            default:
                code = NetworkCode::SOCKET_OPTION_NOT_SUPPORTED;
                network_error(NetworkErrorTable::get(code), "SocketOption not supported!");
                break;

        }

        if (result != 0) {
            NetworkError e = NetworkErrorTable::getLastError();
            network_error(e, "Failed to set socket option");
            code = e.code;
        }

        return code;
    }

    SocketResponse<SOCKET> Socket::open(SocketType type) {
        NetworkCode code = NetworkCode::NONE;

        switch (type) {

            case SocketType::TCP:
                mHandle = ::socket(address.family, SOCK_STREAM, IPPROTO_TCP);
                break;

            case SocketType::UDP:
                mHandle = ::socket(address.family, SOCK_DGRAM, IPPROTO_UDP);
                break;

            default:
                code = NetworkCode::SOCKET_NOT_SUPPORTED;
                network_error(NetworkErrorTable::get(code), "SocketType not supported!");
                break;

        }

        return { code, mHandle };
    }

    SocketResponse<int> Socket::connect(const Address &address) {
        this->address = address;
        return connect();
    }

    SocketResponse<int> Socket::connect() {
        SocketAddress socketAddress = address.getSocketAddress();
        if (socketAddress.valid()) {
            int connections = ::connect(mHandle, socketAddress.value, socketAddress.size);
            return { NetworkCode::NONE, connections };
        } else {
            NetworkError e = NetworkErrorTable::getLastError();
            network_error(e, "Failed to get socket address for connecting");
            return { e.code, 0 };
        }
    }

    NetworkCode Socket::bind(const Address &address) {
        this->address = address;
        return bind();
    }

    NetworkCode Socket::bind() {
        SocketAddress socketAddress = address.getSocketAddress();
        if (socketAddress.valid()) {
            int result = ::bind(mHandle, socketAddress.value, socketAddress.size);
            return static_cast<NetworkCode>(result);
        } else {
            NetworkError e = NetworkErrorTable::getLastError();
            network_error(e, "Failed to get socket address for binding");
            return e.code;
        }
    }

    NetworkCode Socket::listen(const int connectionsCount) {
        bind();

        int result = ::listen(mHandle, connectionsCount);
        if (result != 0) {
            NetworkError e = NetworkErrorTable::getLastError();
            network_error(e, "Failed to listen on socket");
            return e.code;
        }

        return NetworkCode::NONE;
    }

    SocketResponse<Socket*> Socket::accept() {
        SocketAddress acceptedAddress = getSocketAddress();
        if (acceptedAddress.valid()) {
            SOCKET acceptedHandle = ::accept(mHandle, acceptedAddress.value, &acceptedAddress.size);
            return { NetworkCode::NONE, new Socket(acceptedHandle, acceptedAddress) };
        } else {
            NetworkError e = NetworkErrorTable::get(NetworkCode::ADDRESS_INVALID);
            network_error(e, "Failed to get socket address for accepting address");
            return { e.code, null };
        }
    }

    NetworkCode Socket::fetchProfile(Address& address) {
//        if (getnameinfo((sockaddr*)&sockAddr, sizeof(sockAddr), mHost, NI_MAXHOST, mService, NI_MAXSERV, 0) == 0) {
//            info("Host connected on port: {0}", mService);
//        } else {
//            inet_ntop(AF_INET, &sockAddr.sin_addr, mHost, NI_MAXHOST);
//            info("Host connected on port: {0}", ntohs(sockAddr.sin_port));
//        }
        return NetworkCode::NONE;
    }

    SocketResponse<int> Socket::receiveFrom(char* buffer, size_t size, int flags, const sockaddr_in& sockAddr) {
        size_t addressSize = sizeof(sockAddr);
        int bytesReceived = recvfrom(
                mHandle,
                buffer, size,
                flags,
                (sockaddr*)&sockAddr,
                reinterpret_cast<socklen_t *>(&addressSize)
        );

        if (bytesReceived == SOCKET_ERROR) {
            return { NetworkCode::SOCKET, bytesReceived };
        }

        return { NetworkCode::NONE, bytesReceived };
    }

    SocketResponse<int> Socket::receive(char* buffer, size_t size, int flags) {
        int bytesReceived = recv(mHandle, buffer, size, flags);

        if (bytesReceived == SOCKET_ERROR) {
            return { NetworkCode::SOCKET, bytesReceived };
        }

        return { NetworkCode::NONE, bytesReceived };
    }

    SocketResponse<int> Socket::receiveAll(char* buffer, size_t size, int flags) {
        return { NetworkCode::NONE, 0 };
    }

    SocketResponse<int> Socket::send(char* buffer, size_t size, int flags) {
        int bytesSent = ::send(mHandle, buffer, size + 1, flags);

        if (bytesSent == SOCKET_ERROR) {
            return { NetworkCode::SOCKET, bytesSent };
        }

        return { NetworkCode::NONE, bytesSent };
    }

    SocketResponse<int> Socket::sendAll(char* buffer, size_t size, int flags) {
        return { NetworkCode::NONE, 0 };
    }

    SocketResponse<int> Socket::sendTo(char* buffer, size_t size, int flags, sockaddr* sockAddr, size_t addressSize) {
        int bytesSent = ::sendto(mHandle, buffer, size, flags, sockAddr, addressSize);

        if (bytesSent == SOCKET_ERROR) {
            return { NetworkCode::SOCKET, bytesSent };
        }

        return { NetworkCode::NONE, bytesSent };
    }

    NetworkError Socket::getLastError() {
        return NetworkErrorTable::getLastError();
    }

#ifdef WINDOWS

    void Socket::close() {
        closesocket(mHandle);
    }

#elif LINUX

    void Socket::close() {
        close(mHandle);
    }

#endif

}
