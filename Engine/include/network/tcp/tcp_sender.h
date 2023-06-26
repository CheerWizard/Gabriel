#pragma once

#include <network/api/socket.h>
#include <network/core/network_queue.h>

namespace gl {

    struct TCPClient;

    struct GABRIEL_API TCPSender final {

        TCPSender() = default;

        TCPSender(TCPClient* client, NetworkQueue<NetworkStream>* queue)
        : m_client(client), m_queue(queue) {}

        ~TCPSender() {
            stop();
        }

        void run();
        void stop();

    protected:
        virtual void onSendSuccess();
        virtual void onSendFailed(const NetworkError& error);

    private:
        void runImpl();

    private:
        TCPClient* m_client = null;
        bool m_running = false;
        NetworkQueue<NetworkStream>* m_queue = null;
    };

}