#pragma once

#include <primitives.h>

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

}
