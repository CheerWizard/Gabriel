#include <network/tcp/tcp_client.h>

namespace gl {

    TCPClient::TCPClient(const Address& address) {
        connect(address);
    }

    void TCPClient::onCreate() {
        m_socket = new Socket(TCP, V4);
        if (m_socket->invalid()) {
            NetworkError e = m_socket->getLastError();
            network_error(e, "TCPClient: Unable to create a client socket");
        }
    }

    void TCPClient::onDestroy() {
        m_connecting = false;
        delete m_socket;
    }

    void TCPClient::connect(const Address& address) {
        if (!m_connecting) {
            m_connecting = true;
            m_socket->address = address;
            std::thread connectThread([this]() {
                onCreate();
                connectImpl();
                onDestroy();
            });
            connectThread.detach();
        }
    }

    void TCPClient::connectImpl() {
        info("TCPClient: connecting to a server[ip:{0}, port:{1}]", m_socket->address.string, m_socket->address.port);
        auto response = m_socket->connect();

        if (response.code == NetworkCode::SOCKET) {
            NetworkError e = m_socket->getLastError();
            network_error(e, "TCPClient: Failed to connect to a server");
            onConnectionRetry();
        }

        onConnected();
    }

    void TCPClient::onDisconnected() {}

    void TCPClient::onConnected() {
        m_sender = TCPSender(this, &m_queue);
        m_receiver = TCPReceiver(this);

        m_sender.run();
        m_receiver.run();
    }

    void TCPClient::onConnectionRetry() {
        warning("TCPClient: Retry to connect to a server after {0} ms.", m_retrySleepMillis);
        std::this_thread::sleep_for(m_retrySleepMillis);
        connectImpl();
    }

    void TCPClient::disconnect() {
        onDisconnected();
    }

    void TCPClient::connectRetry() {
        onConnectionRetry();
    }

    void TCPClient::reconnect() {
        disconnect();
        connectRetry();
    }

}