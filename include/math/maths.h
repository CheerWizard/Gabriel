#pragma once

#include <core/common.h>

#include <random>

#define PI 3.1415926535897932384626433832795f
#define EPSILON 1e-21f
#define FLOAT_MAX 3.402823466E+38
#define FLOAT_MIN 1.175494351E-38
#define DOUBLE_MAX 1.7976931348623158E+308
#define DOUBLE_MIN 2.2250738585072014E-308

namespace gl {

    bool approximate(float x1, float x2);

    template<typename T>
    void clamp(T& value, T min, T max) {
        if (value < min) {
            value = min;
        }
        else if (value > max) {
            value = max;
        }
    }

    template<typename T>
    T lerp(T a, T b, T f) {
        return a + f * (b - a);
    }

    template<typename T>
    T random(T min, T max) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<T> distr(min, max);
        return distr(gen);
    }

}
