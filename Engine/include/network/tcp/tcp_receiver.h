#pragma once

#include <network/api/socket.h>
#include <network/io/serialization.h>

namespace gl {

    struct TCPClient;

    struct GABRIEL_API TCPReceiver final {

        TCPReceiver() {
            init();
        }

        TCPReceiver(TCPClient* client)
        : m_client(client) {
            init();
        }

        ~TCPReceiver() {
            stop();
            free();
        }

        void run();
        void stop();

    protected:
        virtual void onReceiveFailed(const NetworkError& error);
        virtual void onReceiveSuccess();
        virtual void onReceiveNothing();

    private:
        void init();
        void free();

        void runImpl();

    private:
        TCPClient* m_client = null;
        bool m_running = false;
        char* m_buffer = null;
        size_t m_bufferSize = 1024;
    };

}