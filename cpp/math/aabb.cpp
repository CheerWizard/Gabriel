#include <math/aabb.h>

#include <core/common.h>

namespace gl {

    void AABB::add(const glm::vec3& v) {
        minX = fmin(minX, v.x);
        minY = fmin(minY, v.y);
        minZ = fmin(minZ, v.z);

        maxX = fmax(maxX, v.x);
        maxY = fmax(maxY, v.y);
        maxZ = fmax(maxZ, v.z);
    }

    void AABB::log() const {
        info(
                "X: [" << minX << ',' << maxX << ']' << std::endl <<
                "Y: [" << minY << ',' << maxY << ']' << std::endl <<
                "Z: [" << minZ << ',' << maxZ << ']' << std::endl
        );
    }

    void AABB::update(OrthoMat& mat) const {
        mat.right = maxX;
        mat.left = minX;
        mat.bottom = minY;
        mat.top = maxY;
        mat.zNear = minZ;
        mat.zFar = maxZ;
    }

}