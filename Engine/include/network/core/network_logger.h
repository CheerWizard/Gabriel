#pragma once

#include <network/core/network_error.h>

#ifdef DEBUG

#define network_error(e, ...) error("{0}\nError code: {1}={2}\nError message: {3}", __VA_ARGS__, e.title, e.code, e.message)

#else

#define network_error(code, ...)

#endif