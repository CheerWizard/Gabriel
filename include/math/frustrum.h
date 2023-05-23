#pragma once

namespace gl {

    struct Frustum final {

        Plane topFace;
        Plane bottomFace;

        Plane rightFace;
        Plane leftFace;

        Plane farFace;
        Plane nearFace;

        Frustum() = default;

        Frustum(const PerspectiveMat& perspectiveMat) {
            init(perspectiveMat);
        }

        void init(const PerspectiveMat& perspectiveMat);

        inline Frustum& operator *(const glm::mat4& m) {
//            nearTopLeft     = m * nearTopLeft;
//            nearBottomLeft  = m * nearBottomLeft;
//            nearTopRight    = m * nearTopRight;
//            nearBottomRight = m * nearBottomRight;
//
//            farTopLeft     = m * farTopLeft;
//            farBottomLeft  = m * farBottomLeft;
//            farTopRight    = m * farTopRight;
//            farBottomRight = m * farBottomRight;

            return *this;
        }

        void fill(AABB& aabb) const;

        void log() const;

        Frustum& toWorldSpace(ViewMat& viewMat);
    };

}