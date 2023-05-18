#pragma once

namespace gl {

    struct OrthoMat {
        float left = 0;
        float right = 0;
        float bottom = 0;
        float top = 0;
        float zNear = 0.1f;
        float zFar = 100.0f;

        glm::mat4 init() const;
    };

    struct PerspectiveMat {
        float fovDegree = 45.0f;
        float aspectRatio = 1.0f;
        float zNear = 0.1f;
        float zFar = 100.0f;

        glm::mat4 init() const;
    };

    struct ViewMat {
        glm::vec3 eye = { 0, 0, 0 };
        glm::vec3 center = { 0, 0, 0 };
        glm::vec3 up = { 0, 1, 0 };

        glm::mat4 init() const;
    };

    struct ModelMat {
        glm::fvec3 translation = { 0, 0, 0 };
        glm::fvec3 rotation = { 0, 0, 0 };
        glm::fvec3 scale = { 1, 1, 1 };

        glm::mat4 init() const;
    };

    struct RigidBody {
        glm::fvec3 translation = { 0, 0, 0 };
        glm::fvec3 rotation = { 0, 0, 0 };

        glm::mat4 init() const;
    };

}