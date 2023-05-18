#pragma once

namespace gl {

    struct Plane final {
        float distance;
        glm::vec3 normal;

        Plane(const float distance = 1.0f, const glm::vec3& normal = { 0, 1, 0 })
        : distance(distance), normal(normal) {}
    };

}