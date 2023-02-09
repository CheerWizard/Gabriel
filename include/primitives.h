#pragma once

#include <cstdint>

#define null nullptr

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#include <cassert>
#include <iostream>

#define print(msg) std::cout << msg << std::endl
#define print_err(msg) std::cerr << msg << std::endl

#define COLOR_CLEAR .25f, .25f, 0.25f, 1.0f