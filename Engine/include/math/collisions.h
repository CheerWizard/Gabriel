#pragma once

namespace gl {

    component(RayCollider) {
        Ray ray;

        RayCollider() = default;
        RayCollider(const Ray& ray) : ray(ray) {}
    };

    component(SphereCollider) {
        Sphere sphere;

        SphereCollider() = default;
        SphereCollider(const glm::vec3& center, const float radius) : sphere(center, radius) {}
    };

    struct GABRIEL_API CollisionResponse {
        bool hit = false;
    };

    struct GABRIEL_API RaySphereCollisionResponse : CollisionResponse {
        glm::vec3 p1;
        glm::vec3 p2;
        float d1;
        float d2;
    };

    struct GABRIEL_API CollisionDetection final {
        static RaySphereCollisionResponse test(const RayCollider& rayCollider, const SphereCollider& sphereCollider);
    };

}