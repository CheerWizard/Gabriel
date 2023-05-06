#pragma once

#include <math/raycast.h>
#include <math/math_functions.h>

#include <ecs/component.h>

namespace gl {

    component(RayCollider) {
        glm::vec3 p0 = { 0, 0, 0 }; // start point
        glm::vec3 p1 = { 0, 0, 0 }; // end point

        RayCollider() = default;

        RayCollider(const glm::vec3 &p0, const glm::vec3 &p1)
        : p0(p0), p1(p1) {}

        float length() const;
    };

    component(SphereCollider) {
        glm::vec3 center = { 0, 0, 0 };
        float radius = 1.0f;

        SphereCollider() = default;
        SphereCollider(const glm::vec3& center, float radius)
        : center(center), radius(radius) {}
    };

    struct CollisionResponse {
        bool hit = false;
    };

    struct RaySphereCollisionResponse : CollisionResponse {
        glm::vec3 p1;
        glm::vec3 p2;
        float d1;
        float d2;
    };

    struct CollisionDetection final {
        static RaySphereCollisionResponse test(const RayCollider& ray, const SphereCollider& sphere);
    };

}