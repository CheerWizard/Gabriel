#include "network/core/network_error.h"

namespace gl {

    NetworkError NetworkErrorTable::get(const NetworkCode errorCode) {
        auto it = sErrors.find(errorCode);

        if (it == sErrors.cend()) {
            return sErrors.at(NetworkCode::NOT_IMPLEMENTED);
        }

        return it->second;
    }

    NetworkError NetworkErrorTable::getLastError() {
        int errorCode;

#ifdef WINDOWS

        errorCode = WSAGetLastError();

#elif LINUX

        errorCode = 0;

#endif

        return get(static_cast<NetworkCode>(errorCode));
    }

}