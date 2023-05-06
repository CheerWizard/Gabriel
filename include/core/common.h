#pragma once

#include <core/logger.h>

#include <cstdint>
#include <cassert>

#define null nullptr

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define COLOR_CLEAR 0, 0, 0, 0

#define PI 3.1415926535897932384626433832795f
#define EPSILON 1e-21f
#define FLOAT_MAX 3.402823466E+38
#define FLOAT_MIN 1.175494351E-38
#define DOUBLE_MAX 1.7976931348623158E+308
#define DOUBLE_MIN 2.2250738585072014E-308

namespace math {
    bool approximate(float x1, float x2);
}