#pragma once

#include <core/common.h>

#include <glm/glm.hpp>

#define RAY(Derived) \
Derived() : Ray() {} \
Derived(float x) : Ray(x) {} \
Derived(float x, float y) : Ray(x, y) {} \
Derived(float x, float y, float z) : Ray(x, y, z) {} \
Derived(float x, float y, float z, float w) : Ray(x, y, z, w) {} \
Derived(const glm::vec2& vector) : Ray(vector) {} \
Derived(const glm::vec3& vector) : Ray(vector) {} \
Derived(const glm::vec4& vector) : Ray(vector) {} \

namespace gl {

    struct Ray {
        glm::vec4 vector = { 0, 0, 0, 1 };

        Ray() = default;

        Ray(float x) {
            vector.x = x;
        }

        Ray(float x, float y) {
            vector.x = x;
            vector.y = y;
        }

        Ray(float x, float y, float z) {
            vector.x = x;
            vector.y = y;
            vector.z = z;
        }

        Ray(float x, float y, float z, float w) {
            vector.x = x;
            vector.y = y;
            vector.z = z;
            vector.w = w;
        }

        Ray(const glm::vec2& vector) {
            this->vector.x = vector.x;
            this->vector.y = vector.y;
        }

        Ray(const glm::vec3& vector) {
            this->vector.x = vector.x;
            this->vector.y = vector.y;
            this->vector.z = vector.z;
        }

        Ray(const glm::vec4& vector) {
            this->vector = vector;
        }

        inline float& x() { return vector.x; }
        inline float& y() { return vector.y; }
        inline float& z() { return vector.z; }
        inline float& w() { return vector.w; }

        inline glm::vec2 vec2() { return { vector.x, vector.y }; }
        inline glm::vec3 vec3() { return { vector.x, vector.y, vector.z }; }
    };

    struct LocalRay;
    struct WorldRay;
    struct ViewRay;
    struct ClipRay;
    struct NDCRay;
    struct ScreenRay;

    struct LocalRay : Ray {
        RAY(LocalRay)

        WorldRay worldSpace(const glm::mat4& model);
    };

    struct WorldRay : Ray {
        RAY(WorldRay)

        LocalRay localSpace(const glm::mat4& model);
        ViewRay viewSpace(const glm::mat4& view);
    };

    struct ViewRay : Ray {
        RAY(ViewRay)

        WorldRay worldSpace(const glm::mat4 &view);
        ClipRay clipSpace(float fov, float aspect_ratio);
        ClipRay clipSpace(const glm::mat4& projection);
    };

    struct ClipRay : Ray {
        RAY(ClipRay)

        ViewRay viewSpace(float fov, float aspect_ratio);
        ViewRay viewSpace(const glm::mat4& projection);
        NDCRay ndcSpace();
    };

    struct NDCRay : Ray {
        RAY(NDCRay)

        ClipRay clipSpace();
        ScreenRay screenSpace(int width, int height);
    };

    struct ScreenRay : Ray {
        RAY(ScreenRay)

        NDCRay ndcSpace(int width, int height);
    };

}