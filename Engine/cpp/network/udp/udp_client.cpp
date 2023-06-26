#include <network/udp/udp_client.h>

namespace gl {

    UDPClient::UDPClient(UDPClientListener* listener) : mListener(listener) {
        init();
    }

    UDPClient::UDPClient(const Address& address, UDPClientListener* listener) : mListener(listener) {
        init();
        connect(address);
    }

    UDPClient::~UDPClient() {
        close();
    }

    void UDPClient::init() {
        mSocket = new Socket(UDP, V4);
        if (mSocket->invalid()) {
            int errorCode = mSocket->getLastError();
            error("UDPClient: Unable to create a client socket! Unknown error={0}", errorCode);
//            if (mListener)
//                mListener->onUDPSocketClosed();
        }

        queue = new std::queue<NetworkStream>();
        mSender = new UDPSender(mSocket, queue);
        mReceiver = new UDPReceiver(mSocket);
    }

    void UDPClient::close() {
        mConnecting = false;
        delete mSender;
        delete mReceiver;
        delete mSocket;
        delete queue;
//        if (mListener)
//            mListener->onUDPSocketClosed();
    }

    void UDPClient::connect(const Address& address) {
        if (!mConnecting) {
            mConnecting = true;
            std::async(std::launch::async, &UDPClient::connectImpl, this, address);
        }
    }

    void UDPClient::connectImpl(const Address& address) {
        mSocket->address = address;

        mSender->run();
        mReceiver->run();

        mConnecting = false;
    }

}