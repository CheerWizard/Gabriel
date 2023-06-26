#include <network/tcp/tcp_sender.h>
#include <network/tcp/tcp_client.h>

namespace gl {

    void TCPSender::run() {
        if (!m_running) {
            m_running = true;
            std::thread runThread([this]() {
                runImpl();
            });
            runThread.detach();
        }
    }

    void TCPSender::stop() {
        m_running = false;
    }

    void TCPSender::runImpl() {
        while (m_running) {

            if (m_queue->empty()) {
                std::this_thread::yield();
            }

            NetworkStream& stream = m_queue->front();
            auto response = m_client->send(stream.data(), stream.size() + 1, 0);

            if (response.code == NetworkCode::SOCKET) {
                NetworkError e = NetworkErrorTable::getLastError();
                network_error(e, "TCPSender: Failed to send data");
                onSendFailed(e);
                continue;
            }

            onSendSuccess();

            m_queue->pop();
        }
    }

    void TCPSender::onSendFailed(const NetworkError& error) {

    }

    void TCPSender::onSendSuccess() {
    }

}