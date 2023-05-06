#pragma once

#include <math/matrices.h>

namespace gl {

    struct AABB final {

        float minX = FLT_MAX;
        float maxX = FLT_MIN;
        float minY = FLT_MAX;
        float maxY = FLT_MIN;
        float minZ = FLT_MAX;
        float maxZ = FLT_MIN;

        AABB() = default;

        void add(const glm::vec3& v);

        void log() const;

        void update(OrthoMat& mat) const;
    };

}