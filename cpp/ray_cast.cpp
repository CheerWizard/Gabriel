#include <ray_cast.h>

namespace gl {

    WorldRay LocalRay::world_space(const glm::mat4& model) {
        return { model * vector };
    }

    LocalRay WorldRay::local_space(const glm::mat4& model) {
        return { glm::inverse(model) * vector };
    }

    ViewRay WorldRay::view_space(const glm::mat4& view) {
        return { view * vector };
    }

    WorldRay ViewRay::world_space(const glm::mat4& view) {
        return { glm::inverse(view) * vector };
    }

    ClipRay ViewRay::clip_space(float fov, float aspect_ratio) {
        float focal_length = 1.0f / glm::tan(glm::radians(fov / 2.0f));
        return { vector.x * focal_length, vector.y * focal_length / aspect_ratio, 1.0f, vector.w };
    }

    ClipRay ViewRay::clip_space(const glm::mat4& projection) {
        return { projection * vector };
    }

    ViewRay ClipRay::view_space(float fov, float aspect_ratio) {
        float focal_length = 1.0f / glm::tan(glm::radians(fov / 2.0f));
        return { vector.x / focal_length, vector.y * aspect_ratio / focal_length, 1.0f, vector.w };
    }

    ViewRay ClipRay::view_space(const glm::mat4& projection) {
        return { glm::inverse(projection) * vector };
    }

    NDCRay ClipRay::ndc_space() {
        return { vector.x / vector.w, vector.y / vector.w, vector.z / vector.w, vector.w };
    }

    ClipRay NDCRay::clip_space() {
        return { vector.x * vector.w, vector.y * vector.w, vector.z * vector.w, vector.w };
    }

    ScreenRay NDCRay::screen_space(int width, int height) {
        return { (vector.x + 1) * (float) width / 2, (vector.y + 1) * (float) height / 2, 1.0f, vector.w };
    }

    NDCRay ScreenRay::ndc_space(int width, int height) {
        return { (vector.x * 2 / (float) width) - 1, 1 - (vector.y * 2 / (float) height), 1.0f, vector.w };
    }

}