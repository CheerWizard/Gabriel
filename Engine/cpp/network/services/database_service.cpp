#include <network/services/database_service.h>

namespace gl {

    DatabaseService::DatabaseService(TCPClient* client) : mClient(client) {
        mClient->connect({ localhost, 8080, IPFamily::V4 });
    }

    DatabaseService::~DatabaseService() {
    }

    void DatabaseService::save(BinaryStream& body) {
        std::vector<std::string> headers = {
            "SAVE"
        };
        mClient->queue->emplace(headers, body);
    }

    void DatabaseService::load(BinaryStream& body) {
        std::vector<std::string> headers = {
            "LOAD"
        };
        mClient->queue->emplace(headers, body);
    }

}