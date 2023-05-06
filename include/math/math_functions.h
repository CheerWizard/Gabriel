#pragma once

#include <core/common.h>

#include <random>

namespace gl {

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
