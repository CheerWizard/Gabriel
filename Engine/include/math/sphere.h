#pragma once

namespace gl {

    struct GABRIEL_API Sphere final {
        glm::vec3 center;
        float radius;

        Sphere(const glm::vec3& center = { 0, 0, 0 }, const float radius = 1.0f)
        : center(center), radius(radius) {}
    };

}