#pragma once

#include <cstdint>
#include <cassert>
#include <iostream>

#define null nullptr

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define print(msg) std::cout << msg << std::endl
#define print_err(msg) std::cerr << msg << std::endl

#define COLOR_CLEAR 0, 0, 0, 0

#define PI 3.1415926535897932384626433832795f