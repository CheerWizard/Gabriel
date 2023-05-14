#include <math/raycast.h>

namespace gl {

    WorldRay LocalRay::worldSpace(const glm::mat4& model) {
        return { model * origin };
    }

    LocalRay WorldRay::localSpace(const glm::mat4& model) {
        return { glm::inverse(model) * origin };
    }

    ViewRay WorldRay::viewSpace(const glm::mat4& view) {
        return { view * origin };
    }

    WorldRay ViewRay::worldSpace(const glm::mat4& view) {
        return { glm::inverse(view) * origin };
    }

    ClipRay ViewRay::clipSpace(float fov, float aspectRatio) {
        float focalLength = 1.0f / glm::tan(glm::radians(fov / 2.0f));
        return { origin.x * focalLength, origin.y * focalLength / aspectRatio, 1.0f, origin.w };
    }

    ClipRay ViewRay::clipSpace(const glm::mat4& projection) {
        return { projection * origin };
    }

    ViewRay ClipRay::viewSpace(float fov, float aspectRatio) {
        float focalLength = 1.0f / glm::tan(glm::radians(fov / 2.0f));
        return { origin.x / focalLength, origin.y * aspectRatio / focalLength, 1.0f, origin.w };
    }

    ViewRay ClipRay::viewSpace(const glm::mat4& projection) {
        return { glm::inverse(projection) * origin };
    }

    NDCRay ClipRay::ndcSpace() {
        return { origin.x / origin.w, origin.y / origin.w, origin.z / origin.w, origin.w / origin.w };
    }

    ClipRay NDCRay::clipSpace() {
        return { origin.x * origin.w, origin.y * origin.w, origin.z * origin.w, origin.w };
    }

    ScreenRay NDCRay::screenSpace(int width, int height) {
        return { (origin.x + 1) * (float) width / 2, (origin.y + 1) * (float) height / 2, 1.0f, origin.w };
    }

    NDCRay ScreenRay::ndcSpace(int width, int height) {
        return { (origin.x * 2 / (float) width) - 1, 1 - (origin.y * 2 / (float) height), 1.0f, origin.w };
    }

}