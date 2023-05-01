#include <collisions.h>

namespace gl {

    RaySphereCollisionResponse CollisionDetection::test(const RayCollider &ray, const SphereCollider &sphere) {
        RaySphereCollisionResponse response;

        // conditions
        glm::vec3 p0 = ray.p0;
        glm::vec3 p0p1 = ray.p1 - ray.p0;
        glm::vec3 c = sphere.center;
        float r = sphere.radius;
        glm::vec3 p0c = c - p0;

        // calculate ray time with dot product of P0C and P0P1 vectors
        float t = glm::dot(p0c, p0p1);

        // calculate any point P on ray vector
        glm::vec3 p = p0 + p0p1 * t;

        // calculate X,Y on circle surface
        float y = glm::length(c - p);
        print("collision Y: " << y);
        // intersection exists if Y < R
        if (y < r) {
            response.hit = true;
            float x = sqrtf(r*r - y*y);
            // calculate intersection points P1, P2 and distances D1, D2 to ray origin
            float d1 = t - x;
            float d2 = t + x;
            response.d1 = d1;
            response.d2 = d2;
            response.p1 = p0 + p0p1 * d1;
            response.p2 = p0 + p0p1 * d2;
        }

        return response;
    }

    float RayCollider::length() const {
        return glm::length(p1 - p0);
    }
}