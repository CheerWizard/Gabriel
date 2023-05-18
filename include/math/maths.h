#pragma once

#define PI 3.1415926535897932384626433832795f

#define EPSILON 1e-21f

#define FLOAT_MAX 3.402823466E+38
#define FLOAT_MIN 1.175494351E-38

#define DOUBLE_MAX 1.7976931348623158E+308
#define DOUBLE_MIN 2.2250738585072014E-308

static constexpr float SPHERE_VOLUME_SCALAR = (4.0f / 3.0f) * PI;

namespace gl {

    bool approximate(const float x1, const float x2);

    template<typename T>
    void clamp(T& value, const T min, const T max) {
        if (value < min) {
            value = min;
        }
        else if (value > max) {
            value = max;
        }
    }

    template<typename T>
    T lerp(const T a, const T b, const T f) {
        return a + f * (b - a);
    }

    template<typename T>
    T random(const T min, const T max) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<T> distr(min, max);
        return distr(gen);
    }

    constexpr float sphereVolume(const float radius);

}
