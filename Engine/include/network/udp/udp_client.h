#pragma once

#include <network/udp/udp_sender.h>
#include <network/udp/udp_receiver.h>

namespace gl {

    struct GABRIEL_API UDPClient final {

        UDPClient() = default;

        UDPClient(const Address& address);

        ~UDPClient() = default;

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
        const auto m_retrySleepMillis = 2000ms;
        UDPSender m_sender;
        UDPReceiver m_receiver;
        NetworkQueue<NetworkStream> m_queue;
    };

}