#pragma once

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

    struct GABRIEL_API Ray {
        glm::vec4 origin = { 0, 0, 0, 1 };
        glm::vec4 direction = { 0, 0, 0, 1 };

        Ray() = default;

        Ray(const float x) {
            origin.x = x;
        }

        Ray(const float x, const float y) {
            origin.x = x;
            origin.y = y;
        }

        Ray(const float x, const float y, const float z) {
            origin.x = x;
            origin.y = y;
            origin.z = z;
        }

        Ray(const float x, const float y, const float z, const float w) {
            origin.x = x;
            origin.y = y;
            origin.z = z;
            origin.w = w;
        }

        Ray(const glm::vec2& vector) {
            this->origin.x = vector.x;
            this->origin.y = vector.y;
        }

        Ray(const glm::vec3& vector) {
            this->origin.x = vector.x;
            this->origin.y = vector.y;
            this->origin.z = vector.z;
        }

        Ray(const glm::vec4& vector) {
            this->origin = vector;
        }

        inline float& x() { return origin.x; }
        inline float& y() { return origin.y; }
        inline float& z() { return origin.z; }
        inline float& w() { return origin.w; }

        [[nodiscard]] inline glm::vec2 vec2() const { return {origin.x, origin.y }; }
        [[nodiscard]] inline glm::vec3 vec3() const { return {origin.x, origin.y, origin.z }; }
    };

    struct LocalRay;
    struct WorldRay;
    struct ViewRay;
    struct ClipRay;
    struct NDCRay;
    struct ScreenRay;

    struct GABRIEL_API LocalRay : Ray {
        RAY(LocalRay)

        WorldRay worldSpace(const glm::mat4& model);
    };

    struct GABRIEL_API WorldRay : Ray {
        RAY(WorldRay)

        LocalRay localSpace(const glm::mat4& model);
        ViewRay viewSpace(const glm::mat4& view);
    };

    struct GABRIEL_API ViewRay : Ray {
        RAY(ViewRay)

        WorldRay worldSpace(const glm::mat4 &view);
        ClipRay clipSpace(float fov, float aspect_ratio);
        ClipRay clipSpace(const glm::mat4& projection);
    };

    struct GABRIEL_API ClipRay : Ray {
        RAY(ClipRay)

        ViewRay viewSpace(float fov, float aspect_ratio);
        ViewRay viewSpace(const glm::mat4& projection);
        NDCRay ndcSpace();
    };

    struct GABRIEL_API NDCRay : Ray {
        RAY(NDCRay)

        ClipRay clipSpace();
        ScreenRay screenSpace(int width, int height);
    };

    struct GABRIEL_API ScreenRay : Ray {
        RAY(ScreenRay)

        NDCRay ndcSpace(int width, int height);
    };

}