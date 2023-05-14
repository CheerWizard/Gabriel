#include <math/collisions.h>

namespace gl {

    RaySphereCollisionResponse CollisionDetection::test(const RayCollider& rayCollider, const SphereCollider& sphereCollider) {
        RaySphereCollisionResponse response;

        // conditions
        glm::vec3 p0 = rayCollider.ray.origin;
        glm::vec3 p0p1 = rayCollider.ray.direction;
        glm::vec3 c = sphereCollider.sphere.center;
        float r = sphereCollider.sphere.radius;
        glm::vec3 p0c = c - p0;

        // calculate rayCollider time with dot product of P0C and P0P1 vectors
        float t = glm::dot(p0c, p0p1);

        // calculate any point P on rayCollider vector
        glm::vec3 p = p0 + p0p1 * t;

        // calculate X,Y on circle surface
        // intersection exists if Y < R
        float y = glm::length(c - p);
        if (y < r) {
            response.hit = true;
            float x = sqrtf(r*r - y*y);
            // calculate intersection points P1, P2 and distances D1, D2 to rayCollider origin
            float d1 = t - x;
            float d2 = t + x;
            response.d1 = d1;
            response.d2 = d2;
            response.p1 = p0 + p0p1 * d1;
            response.p2 = p0 + p0p1 * d2;
        }

        return response;
    }

}