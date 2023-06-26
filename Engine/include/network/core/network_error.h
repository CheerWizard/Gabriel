#pragma once

#include <network/core/network_codes.h>

namespace gl {

    struct GABRIEL_API NetworkError final {
        NetworkCode code = NetworkCode::NONE;
        const char* title;
        const char* message;

        NetworkError() = default;

        NetworkError(const char* title, const char* message)
        : title(title), message(message) {}

        NetworkError(const NetworkCode code, const char* title, const char* message)
        : code(code), title(title), message(message) {}
    };

    struct GABRIEL_API NetworkErrorTable final {

        static NetworkError get(const NetworkCode errorCode);
        static NetworkError getLastError();

    private:
        inline static const std::unordered_map<NetworkCode, NetworkError> sErrors = {
            { NetworkCode::NONE, { "NONE", "No errors" } },
            { NetworkCode::NOT_IMPLEMENTED, { "NOT_IMPLEMENTED", "Error code is not implemented" } },
            { NetworkCode::SOCKET, { "SOCKET", "Error caused by a socket" } },
            { NetworkCode::SOCKET_INVALID, { "SOCKET_INVALID", "Invalid socket handle" } },
            { NetworkCode::OUT_OF_MEMORY, { "OUT_OF_MEMORY", "Network memory is out of range" } },
            { NetworkCode::INVALID_PARAMETER, { "INVALID_PARAMETER", "Call parameter is invalid" } },
//            { NetworkCode::INVALID_EVENT, { "INVALID_EVENT", "Call event is invalid" } },
            { NetworkCode::OPERATION_ABORTED, { "OPERATION_ABORTED", "Operation has been aborted" } },
            { NetworkCode::IO_INCOMPLETE, { "IO_INCOMPLETE", "Network I/O operations are incomplete" } },
            { NetworkCode::IO_PENDING, { "IO_PENDING", "Network I/O operations are in progress or overlapped" } },
            { NetworkCode::CALL_INTERRUPTED, { "CALL_INTERRUPTED", "Call has been interrupted by something else" } },
            { NetworkCode::INVALID_FILE, { "INVALID_FILE", "File handle is invalid" } },
            { NetworkCode::PERMISSION_DENIED, { "PERMISSION_DENIED", "Permission to resource has been denied" } },
            { NetworkCode::ADDRESS_INVALID, { "ADDRESS_INVALID", "Socket address is invalid for machine" } },
            { NetworkCode::INVALID_ARGUMENT, { "INVALID_ARGUMENT", "Call argument is invalid" } },
            { NetworkCode::TOO_MANY_OPEN_FILES, { "TOO_MANY_OPEN_FILES", "Too many files are opened in parallel" } },
            { NetworkCode::RESOURCE_TEMP_UNAVAILABLE, { "RESOURCE_TEMP_UNAVAILABLE", "Resource is temporary unavailable" } },
            { NetworkCode::NOW_IN_PROGRESS, { "NOW_IN_PROGRESS", "Call or socket is in progress" } },
            { NetworkCode::ALREADY_IN_PROGRESS, { "ALREADY_IN_PROGRESS", "Call or socket are already in progress" } },
            { NetworkCode::INVALID_OPERATION, { "INVALID_OPERATION", "Call operation is invalid" } },
            { NetworkCode::DESTINATION_ADDRESS_REQUIRED, { "DESTINATION_ADDRESS_REQUIRED", "Destination address is required" } },
            { NetworkCode::TOO_LONG_MESSAGE, { "TOO_LONG_MESSAGE", "Message is too long" } },
            { NetworkCode::WRONG_PROTOCOL_TYPE, { "WRONG_PROTOCOL_TYPE", "Protocol type is wrong" } },
            { NetworkCode::WRONG_PROTOCOL_OPTION, { "WRONG_PROTOCOL_OPTION", "Protocol option is wrong" } },
            { NetworkCode::PROTOCOL_NOT_SUPPORTED, { "PROTOCOL_NOT_SUPPORTED", "Protocol is not supported" } },
            { NetworkCode::SOCKET_NOT_SUPPORTED, { "SOCKET_NOT_SUPPORTED", "Socket is not supported" } },
            { NetworkCode::OPERATION_NOT_SUPPORTED, { "OPERATION_NOT_SUPPORTED", "Operation is not supported" } },
            { NetworkCode::PROTOCOL_FAMILY_NOT_SUPPORTED, { "PROTOCOL_FAMILY_NOT_SUPPORTED", "Protocol family is not supported" } },
            { NetworkCode::ADDRESS_FAMILY_NOT_SUPPORTED, { "ADDRESS_FAMILY_NOT_SUPPORTED", "Address family is not supported" } },
            { NetworkCode::ADDRESS_IN_USE, { "ADDRESS_IN_USE", "Address is in use" } },
            { NetworkCode::ADDRESS_NOT_AVAILABLE, { "ADDRESS_NOT_AVAILABLE", "Address is not available" } },
            { NetworkCode::NETWORK_DEAD, { "NETWORK_DEAD", "Network is dead" } },
            { NetworkCode::NETWORK_UNREACHABLE, { "NETWORK_UNREACHABLE", "Network is unreachable" } },
            { NetworkCode::NETWORK_DROP_CONNECTION_ON_RESET, { "NETWORK_DROP_CONNECTION_ON_RESET", "Network drops it's connection on reset" } },
            { NetworkCode::CONNECTION_ABORT, { "CONNECTION_ABORT", "Connection has been aborted" } },
            { NetworkCode::CONNECTION_RESET, { "CONNECTION_RESET", "Connection has been reset" } },
            { NetworkCode::BUFFER_NO_SPACE, { "BUFFER_NO_SPACE", "Network buffer has no available space" } },
            { NetworkCode::SOCKET_CONNECTED, { "SOCKET_CONNECTED", "Socket is connected" } },
            { NetworkCode::SOCKET_NOT_CONNECTED, { "SOCKET_NOT_CONNECTED", "Socket is not connected" } },
            { NetworkCode::SOCKET_SHUTDOWN, { "SOCKET_SHUTDOWN", "Socket call performed on dead socket" } },
            { NetworkCode::TOO_MANY_REFERENCES, { "TOO_MANY_REFERENCES", "Too many references" } },
            { NetworkCode::CONNECTION_TIMEOUT, { "CONNECTION_TIMEOUT", "Connection to socket has been timed out" } },
            { NetworkCode::CONNECTION_REFUSED, { "CONNECTION_REFUSED", "Connection to socket has been refused" } },
            { NetworkCode::NAME_CANNOT_TRANSLATE, { "NAME_CANNOT_TRANSLATE", "Can not translate name" } },
            { NetworkCode::NAME_TOO_LONG, { "NAME_TOO_LONG", "Name is too long" } },
            { NetworkCode::HOST_SHUTDOWN, { "HOST_SHUTDOWN", "Host has been shutted down" } },
            { NetworkCode::HOST_NO_ROUTE, { "HOST_NO_ROUTE", "Can't find available route to host" } },
            { NetworkCode::TOO_MANY_PROCESSES, { "TOO_MANY_PROCESSES", "Too many processes are running on network machine" } },
            { NetworkCode::QUOTA_USER_EXCEEDED, { "QUOTA_USER_EXCEEDED", "User quota has been exceeded" } },
            { NetworkCode::QUOTA_DISK_EXCEEDED, { "QUOTA_DISK_EXCEEDED", "Disk quota has been exceeded" } },
            { NetworkCode::FILE_HANDLE_STALE, { "FILE_HANDLE_STALE", "Staling file handle" } },
            { NetworkCode::ITEM_IS_REMOTE, { "ITEM_IS_REMOTE", "Item is remote" } },
            { NetworkCode::NETWORK_SUBSYSTEM_NOT_AVAILABLE, { "NETWORK_SUBSYSTEM_NOT_AVAILABLE", "Network subsystem is not available" } },
            { NetworkCode::LIBRARY_VERSION_NOT_SUPPORTED, { "LIBRARY_VERSION_NOT_SUPPORTED", "Socket library version is not supported" } },
            { NetworkCode::LIBRARY_NOT_INITIALIZED, { "LIBRARY_NOT_INITIALIZED", "Socket library is not initialized" } },
            { NetworkCode::SHUTDOWN_IN_PROGRESS, { "SHUTDOWN_IN_PROGRESS", "Library shutdown is in progress" } },
            { NetworkCode::NO_RESULTS, { "NO_RESULTS", "No results" } },
            { NetworkCode::CALL_CANCEL, { "CALL_CANCEL", "Call has been cancelled" } },
            { NetworkCode::CALL_INVALID_TABLE, { "CALL_INVALID_TABLE", "Call procedures table is invalid" } },
            { NetworkCode::SERVICE_PROVIDER_INVALID, { "SERVICE_PROVIDER_INVALID", "Service provider is invalid" } },
            { NetworkCode::SERVICE_PROVIDER_NOT_INITIALIZED, { "SERVICE_PROVIDER_NOT_INITIALIZED", "Service provider is not initialized" } },
            { NetworkCode::SYSTEM_CALL_FAILURE, { "SYSTEM_CALL_FAILURE", "System call failure" } },
            { NetworkCode::SERVICE_NOT_FOUND, { "SERVICE_NOT_FOUND", "Service is not found" } },
            { NetworkCode::TYPE_NOT_FOUND, { "TYPE_NOT_FOUND", "Class type is not found" } },
            { NetworkCode::QUERY_REFUSED, { "QUERY_REFUSED", "Query to database has been refused" } },
            { NetworkCode::HOST_ISNT_FOUND, { "HOST_ISNT_FOUND", "Unable to find a host" } }
        };

    };

}