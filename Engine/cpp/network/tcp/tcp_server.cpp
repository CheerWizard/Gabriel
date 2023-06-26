#include <network/tcp/tcp_server.h>

namespace gl {

    TCPServer::TCPServer(const Address& address) {
        run(address);
    }

    void TCPServer::onCreate() {
        m_connectionSocket = new Socket(TCP, V4);

        if (m_connectionSocket->invalid()) {
            NetworkError e = m_connectionSocket->getLastError();
            network_error(e, "TCPServer: Unable to create a listening socket");
        }

        m_buffer = static_cast<char*>(malloc(m_bufferSize));
    }

    void TCPServer::onDestroy() {
        free(m_buffer);
        delete m_socket;
    }

    void TCPServer::run(const Address& address) {
        if (!m_listening && !m_socket->invalid()) {
            m_listening = true;
            m_connectionSocket->address = address;
            std::thread runThread([this]() {
                onCreate();
                listenImpl();
                onDestroy();
            });
            runThread.detach();
        }
    }

    void TCPServer::listenImpl() {

        // listen to socket address
        m_connectionSocket->listen(SOMAXCONN);

        // waiting for connection with client socket
        auto acceptResponse = m_connectionSocket->accept();
        m_socket = acceptResponse.value;
        if (acceptResponse.value->invalid()) {
            error("TCPServer: Unable to accept a client connection. Retry...");
            onConnectionRetry();
        }

        Address profileAddress = m_connectionSocket->address;
        m_socket->fetchProfile(profileAddress);

        onConnected();

        m_connectionSocket->close();

        runImpl();

        onDisconnected();
    }

    void TCPServer::onConnected() {}

    void TCPServer::onDisconnected() {}

    void TCPServer::onConnectionRetry() {
        listenImpl();
    }

    void TCPServer::runImpl() {
        while (m_listening) {
            memset(m_buffer, 0, m_bufferSize);

            // receive buffer from client
            auto clientResponse = m_socket->receive(m_buffer, m_bufferSize, 0);
            info("TCPServer: Received buffer \nsize: {0}", clientResponse.value);

            // check socket error
            if (clientResponse.code == NetworkCode::SOCKET) {
                error("TCPServer: Receiver failed. \nsize: {0}", clientResponse.value);
                onReceiveFailed(clientResponse);
                continue;
            }

            // check client connection
            else if (clientResponse.value == 0) {
                warning("TCPServer: No response from client");
                onReceiveNothing(clientResponse);
                continue;
            }

            onReceiveSuccess(clientResponse);
        }
    }

    void TCPServer::stop() {
        m_listening = false;
    }

    void TCPServer::send(char* buffer, size_t size, int flags) {
        info("TCPServer: Send data to a client \nsize:", size);
        m_socket->send(buffer, size + 1, flags);
    }

    void TCPServer::onReceiveSuccess(const SocketResponse<int>& clientResponse) {
        send(m_buffer, clientResponse.value, 0);
    }

    void TCPServer::onReceiveFailed(const SocketResponse<int> &clientResponse) {}

    void TCPServer::onReceiveNothing(const SocketResponse<int> &clientResponse) {}

}