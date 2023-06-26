#include "math/maths.h"

namespace gl {

    bool approximate(const float x1, const float x2) {
        return fabs(x1 - x2) < EPSILON;
    }

    constexpr float sphereVolume(const float radius) {
        return SPHERE_VOLUME_SCALAR * radius * radius * radius;
    }

}