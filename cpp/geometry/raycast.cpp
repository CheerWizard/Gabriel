#include <math/raycast.h>

namespace gl {

    WorldRay LocalRay::worldSpace(const glm::mat4& model) {
        return { model * vector };
    }

    LocalRay WorldRay::localSpace(const glm::mat4& model) {
        return { glm::inverse(model) * vector };
    }

    ViewRay WorldRay::viewSpace(const glm::mat4& view) {
        return { view * vector };
    }

    WorldRay ViewRay::worldSpace(const glm::mat4& view) {
        return { glm::inverse(view) * vector };
    }

    ClipRay ViewRay::clipSpace(float fov, float aspectRatio) {
        float focalLength = 1.0f / glm::tan(glm::radians(fov / 2.0f));
        return { vector.x * focalLength, vector.y * focalLength / aspectRatio, 1.0f, vector.w };
    }

    ClipRay ViewRay::clipSpace(const glm::mat4& projection) {
        return { projection * vector };
    }

    ViewRay ClipRay::viewSpace(float fov, float aspectRatio) {
        float focalLength = 1.0f / glm::tan(glm::radians(fov / 2.0f));
        return { vector.x / focalLength, vector.y * aspectRatio / focalLength, 1.0f, vector.w };
    }

    ViewRay ClipRay::viewSpace(const glm::mat4& projection) {
        return { glm::inverse(projection) * vector };
    }

    NDCRay ClipRay::ndcSpace() {
        return { vector.x * vector.w, vector.y * vector.w, vector.z * vector.w, vector.w };
    }

    ClipRay NDCRay::clipSpace() {
        return { vector.x / vector.w, vector.y / vector.w, vector.z / vector.w, vector.w };
    }

    ScreenRay NDCRay::screenSpace(int width, int height) {
        return { (vector.x + 1) * (float) width / 2, (vector.y + 1) * (float) height / 2, 1.0f, vector.w };
    }

    NDCRay ScreenRay::ndcSpace(int width, int height) {
        return { (vector.x * 2 / (float) width) - 1, 1 - (vector.y * 2 / (float) height), 1.0f, vector.w };
    }

}