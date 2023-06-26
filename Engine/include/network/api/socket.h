#pragma once

#include <network/api/address.h>

namespace gl {

    enum SocketOption {
        TCP_NoDelay          // TRUE - disable Nagle's algorithm
    };

    enum SocketType {
        TCP,
        UDP
    };

    template<typename T>
    struct GABRIEL_API SocketResponse {
        NetworkCode code;
        T value;

        SocketResponse() = default;
        SocketResponse(const NetworkCode code, const T& value) : code(code), value(value) {}
    };

    struct GABRIEL_API Socket final {
        Address address;

        Socket();
        Socket(SOCKET handle);
        Socket(SOCKET handle, const SocketAddress& socketAddress);
        Socket(SocketType type, IPFamily ipFamily, const std::vector<SocketOption>& options = {SocketOption::TCP_NoDelay });
        ~Socket();

        inline const SOCKET& getHandle() const {
            return mHandle;
        }

        inline bool invalid() const {
            return mHandle == NetworkCode::SOCKET_INVALID || mHandle == NetworkCode::SOCKET;
        }

        inline SocketAddress getSocketAddress() {
            return address.getSocketAddress();
        }

        SocketResponse<SOCKET> open(SocketType type);
        void close();

        static NetworkError getLastError();

        SocketResponse<int> connect(const Address& address);
        SocketResponse<int> connect();

        NetworkCode bind(const Address& address);
        NetworkCode bind();

        NetworkCode listen(int connectionsCount);

        SocketResponse<Socket*> accept();

        NetworkCode fetchProfile(Address& address);

        SocketResponse<int> receiveFrom(char* buffer, size_t size, int flags, const sockaddr_in& sockAddr);

        SocketResponse<int> receive(char* buffer, size_t size, int flags);

        SocketResponse<int> receiveAll(char* buffer, size_t size, int flags);

        SocketResponse<int> sendTo(char* buffer, size_t size, int flags, sockaddr* sockAddr, size_t addressSize);

        SocketResponse<int> send(char* buffer, size_t size, int flags = 0);

        SocketResponse<int> sendAll(char* buffer, size_t size, int flags = 0);

        NetworkCode setOption(SocketOption socketOption, BOOL value);

    private:
        SOCKET mHandle = 0;
    };

}