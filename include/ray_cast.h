#pragma once

#include <primitives.h>

#include <glm/glm.hpp>

#define ray_construct(Derived) \
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
        ray_construct(LocalRay)

        WorldRay world_space(const glm::mat4& model);
    };

    struct WorldRay : Ray {
        ray_construct(WorldRay)

        LocalRay local_space(const glm::mat4& model);
        ViewRay view_space(const glm::mat4& view);
    };

    struct ViewRay : Ray {
        ray_construct(ViewRay)

        WorldRay world_space(const glm::mat4 &view);
        ClipRay clip_space(float fov, float aspect_ratio);
        ClipRay clip_space(const glm::mat4& projection);
    };

    struct ClipRay : Ray {
        ray_construct(ClipRay)

        ViewRay view_space(float fov, float aspect_ratio);
        ViewRay view_space(const glm::mat4& projection);
        NDCRay ndc_space();
    };

    struct NDCRay : Ray {
        ray_construct(NDCRay)

        ClipRay clip_space();
        ScreenRay screen_space(int width, int height);
    };

    struct ScreenRay : Ray {
        ray_construct(ScreenRay)

        NDCRay ndc_space(int width, int height);
    };

}