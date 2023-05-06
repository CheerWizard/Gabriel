#pragma once

#include <math/aabb.h>

#define print_vec4(name, v) info(name << "[x:" << v.x << ",y:" << v.y << ",z:" << v.z << ",w:" << v.w << ']')

namespace gl {

    struct Frustum final {

        glm::vec4 nearTopLeft;
        glm::vec4 nearBottomLeft;
        glm::vec4 nearTopRight;
        glm::vec4 nearBottomRight;

        glm::vec4 farTopLeft;
        glm::vec4 farBottomLeft;
        glm::vec4 farTopRight;
        glm::vec4 farBottomRight;

        Frustum() = default;

        Frustum(const PerspectiveMat& perspectiveMat) {
            init(perspectiveMat);
        }

        void init(const PerspectiveMat& perspectiveMat);

        inline Frustum& operator *(const glm::mat4& m) {
            nearTopLeft     = m * nearTopLeft;
            nearBottomLeft  = m * nearBottomLeft;
            nearTopRight    = m * nearTopRight;
            nearBottomRight = m * nearBottomRight;

            farTopLeft     = m * farTopLeft;
            farBottomLeft  = m * farBottomLeft;
            farTopRight    = m * farTopRight;
            farBottomRight = m * farBottomRight;

            return *this;
        }

        void fill(AABB& aabb) const;

        void log() const;

        Frustum& toWorldSpace(const ViewMat& viewMat);
    };

}