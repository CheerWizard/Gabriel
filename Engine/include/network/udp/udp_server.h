#pragma once

#include <network/api/socket.h>
#include <network/io/serialization.h>

namespace gl {

    struct GABRIEL_API UDPServer {

        UDPServer() = default;

        UDPServer(const Address& address);

        ~UDPServer() = default;

        void run(const Address& address);

        void stop();

    protected:
        virtual void onCreate();
        virtual void onDestroy();
        virtual void onConnected();
        virtual void onDisconnected();
        virtual void onReceiveSuccess(const SocketResponse<int>& clientResponse);
        virtual void onReceiveFailed(const SocketResponse<int>& clientResponse);
        virtual void onReceiveNothing(const SocketResponse<int>& clientResponse);
        virtual void onConnectionRetry();

        void send(char* buffer, size_t size, int flags);

    private:
        void listenImpl();
        void runImpl();

    private:
        Socket* m_connectionSocket = null;
        Socket* m_socket = null;
        bool m_listening = false;
        char* m_buffer = null;
        size_t m_bufferSize = 4096;
    };

}