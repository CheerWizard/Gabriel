#pragma once

#include <cstdint>
#include <cassert>
#include <iostream>

#define null nullptr

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define print(msg) std::cout << __FUNCTION__ << ": " << msg << std::endl

#define print_err(msg) \
std::cerr << __FUNCTION__ << ": " << msg << std::endl; \
std::cerr << "Error in file: " << __FILE__ << " function: " << __FUNCTION__ << " line: " << __LINE__ << std::endl

#define print_err_tag(msg, file, function, line) \
std::cerr << function << ": " << msg << std::endl; \
std::cerr << "Error in file: " << file << " function: " << function << " line: " << line << std::endl

#define COLOR_CLEAR 0, 0, 0, 0

#define PI 3.1415926535897932384626433832795f