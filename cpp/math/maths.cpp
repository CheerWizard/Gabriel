#include <math/maths.h>

namespace gl {

    bool approximate(float x1, float x2) {
        return fabs(x1 - x2) < EPSILON;
    }

}