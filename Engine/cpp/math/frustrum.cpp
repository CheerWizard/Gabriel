#include "math/frustrum.h"
#include "core/common.h"

namespace gl {

    void Frustum::init(const PerspectiveMat& perspectiveMat) {
        float aspectRatio = perspectiveMat.aspectRatio;

        float tanHalfFOV = tanf(glm::radians(perspectiveMat.fovDegree / 2.0f));

        float zNear = perspectiveMat.zNear;
        float xNear = zNear * tanHalfFOV;
        float yNear = zNear * tanHalfFOV * aspectRatio;

        float zFar = perspectiveMat.zFar;
        float xFar = zFar * tanHalfFOV;
        float yFar = zFar * tanHalfFOV * aspectRatio;

//        nearTopLeft     = { -xNear, yNear, zNear, 1.0f };
//        nearBottomLeft  = { -xNear, -yNear, zNear, 1.0f };
//        nearTopRight    = { xNear, yNear, zNear, 1.0f };
//        nearBottomRight = { xNear, -yNear, zNear, 1.0f };

//        farTopLeft     = { -xFar, yFar, zFar, 1.0f };
//        farBottomLeft  = { -xFar, -yFar, zFar, 1.0f };
//        farTopRight    = { xFar, yFar, zFar, 1.0f };
//        farBottomRight = { xFar, -yFar, zFar, 1.0f };
    }

    void Frustum::fill(AABB& aabb) const {
//        aabb.add(nearTopLeft);
//        aabb.add(nearBottomLeft);
//        aabb.add(nearTopRight);
//        aabb.add(nearBottomRight);

//        aabb.add(farTopLeft);
//        aabb.add(farBottomLeft);
//        aabb.add(farTopRight);
//        aabb.add(farBottomRight);
    }

    void Frustum::log() const {
//        printVec4("NearTopLeft", nearTopLeft);
//        printVec4("NearBottomLeft", nearBottomLeft);
//        printVec4("NearTopRight", nearTopRight);
//        printVec4("NearBottomRight", nearBottomRight);

//        printVec4("FarTopLeft", farTopLeft);
//        printVec4("FarBottomLeft", farBottomLeft);
//        printVec4("FarTopRight", farTopRight);
//        printVec4("FarBottomRight", farBottomRight);
    }

    Frustum& Frustum::toWorldSpace(ViewMat& viewMat) {
        return *this * glm::inverse(viewMat.init());
    }

}