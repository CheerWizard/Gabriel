#pragma once

#include <core/logger.h>

#include <cstdint>
#include <cassert>
#include <cstdarg>
#include <memory>

#define null nullptr

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define STR(s1, s2) s1 ## s2

template<typename T>
using Scope = std::unique_ptr<T>;

template<typename T, typename ... Args>
constexpr Scope<T> createScope(Args&& ... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T, typename ... Args>
constexpr Ref<T> createRef(Args&& ... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using Weak = std::weak_ptr<T>;

template<typename T, typename ... Args>
constexpr Weak<T> createWeak(Args&& ... args) {
    return std::weak_ptr<T>(std::forward<Args>(args)...);
}


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