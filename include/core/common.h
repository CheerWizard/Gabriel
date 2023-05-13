#pragma once

#include <core/logger.h>

#include <cstdint>
#include <cassert>
#include <cstdarg>

#define null nullptr

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define STR(s1, s2) s1 ## s2
#define COLOR_CLEAR 0, 0, 0, 0
