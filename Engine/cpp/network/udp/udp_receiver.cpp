#include <network/udp/udp_receiver.h>
#include <network/udp/udp_client.h>

namespace gl {

    void UDPReceiver::init() {
        m_buffer = static_cast<char*>(malloc(m_bufferSize));
    }

    void UDPReceiver::free() {
        ::free(m_buffer);
    }

    void UDPReceiver::run() {
        if (!m_running) {
            m_running = true;
            std::thread runThread([this]() {
                runImpl();
            });
            runThread.detach();
        }
    }

    void UDPReceiver::runImpl() {
        while (m_running) {

            memset(m_buffer, 0, m_bufferSize);

            auto response = m_client->receive(m_buffer, m_bufferSize, 0);

            if (response.code == NetworkCode::SOCKET) {
                NetworkError e = NetworkErrorTable::getLastError();
                network_error(e, "UDPReceiver: Failed to receive data");
                onReceiveFailed(e);
                continue;
            }

            else if (response.value == 0) {
                warning("UDPReceiver: No data received");
                onReceiveNothing();
                continue;
            }

            else if (response.value > 0) {
                info("UDPReceiver: Received buffer \nsize: {0}", response.value);
                onReceiveSuccess();
            }
        }
    }

    void UDPReceiver::stop() {
        m_running = false;
    }

    void UDPReceiver::onReceiveFailed(const NetworkError& error) {
        switch (error.code) {

            case NetworkCode::CONNECTION_ABORT:
                m_client->reconnect();
                break;

            case NetworkCode::CONNECTION_REFUSED:
                m_client->reconnect();
                break;

            case NetworkCode::NETWORK_DROP_CONNECTION_ON_RESET:
                m_client->reconnect();
                break;

            default:
                break;

        }
    }

    void UDPReceiver::onReceiveSuccess() {}

    void UDPReceiver::onReceiveNothing() {}

}