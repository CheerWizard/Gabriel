#include <network/udp/udp_sender.h>
#include <network/udp/udp_client.h>

namespace gl {

    void UDPSender::run() {
        if (!m_running) {
            m_running = true;
            std::thread runThread([this]() {
                runImpl();
            });
            runThread.detach();
        }
    }

    void UDPSender::stop() {
        m_running = false;
    }

    void UDPSender::runImpl() {
        while (m_running) {

            if (m_queue->empty()) {
                std::this_thread::yield();
            }

            NetworkStream& stream = m_queue->front();
            auto response = m_client->send(stream.data(), stream.size() + 1, 0);

            if (response.code == NetworkCode::SOCKET) {
                NetworkError e = NetworkErrorTable::getLastError();
                network_error(e, "UDPSender: Failed to send data");
                onSendFailed(e);
                continue;
            }

            onSendSuccess();

            m_queue->pop();
        }
    }

    void UDPSender::onSendFailed(const NetworkError& error) {

    }

    void UDPSender::onSendSuccess() {
    }

}