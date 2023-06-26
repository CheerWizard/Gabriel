#pragma once

#include <network/api/socket.h>
#include <network/core/network_queue.h>

namespace gl {

    struct UDPClient;

    struct GABRIEL_API UDPSender final {

        UDPSender() = default;

        UDPSender(UDPClient* client, NetworkQueue<NetworkStream>* queue)
        : m_client(client), m_queue(queue) {}

        ~UDPSender() {
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
        UDPClient* m_client = null;
        bool m_running = false;
        NetworkQueue<NetworkStream>* m_queue = null;
    };

}