#pragma once

#include <network/tcp/tcp_sender.h>
#include <network/tcp/tcp_receiver.h>

namespace gl {

    struct GABRIEL_API TCPClient final {

        TCPClient() = default;

        TCPClient(const Address& address);

        ~TCPClient() = default;

        void connect(const Address& address);

        void disconnect();

        void connectRetry();

        void reconnect();

        inline SocketResponse<int> send(char* data, size_t size, int flags) {
            return m_socket->send(data, size, flags);
        }

        inline SocketResponse<int> receive(char* data, size_t size, int flags) {
            return m_socket->receive(data, size, flags);
        }

    protected:
        virtual void onCreate();
        virtual void onDestroy();
        virtual void onConnected();
        virtual void onDisconnected();
        virtual void onConnectionRetry();

    private:
        void connectImpl();

    protected:
        Socket* m_socket = null;
        bool m_connecting = false;
        const TimeMillis m_retrySleepMillis = 2000ms;
        TCPSender m_sender;
        TCPReceiver m_receiver;
        NetworkQueue<NetworkStream> m_queue;
    };

}