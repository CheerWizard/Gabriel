#pragma once

namespace gl {

    struct OrthoMat {
        float left = 0;
        float right = 0;
        float bottom = 0;
        float top = 0;
        float zNear = 0.1f;
        float zFar = 100.0f;
        glm::mat4 value;

        glm::mat4& init();
    };

    struct PerspectiveMat {
        float fovDegree = 45.0f;
        float aspectRatio = 1.0f;
        float zNear = 0.1f;
        float zFar = 100.0f;
        glm::mat4 value;

        glm::mat4& init();
    };

    struct ViewMat {
        glm::vec3 eye = { 0, 0, 0 };
        glm::vec3 center = { 0, 0, 0 };
        glm::vec3 up = { 0, 1, 0 };
        glm::mat4 value;

        glm::mat4& init();
    };

    struct ModelMat {
        glm::fvec3 translation = { 0, 0, 0 };
        glm::fvec3 rotation = { 0, 0, 0 };
        glm::fvec3 scale = { 1, 1, 1 };
        glm::mat4 value;

        glm::mat4& init();
    };

    struct RigidBody {
        glm::fvec3 translation = { 0, 0, 0 };
        glm::fvec3 rotation = { 0, 0, 0 };
        glm::mat4 value;

        glm::mat4& init();
    };

    struct Model2dMat {
        glm::fvec2 translation = { 0, 0 };
        float rotation = 0;
        glm::fvec2 scale = { 1, 1 };
        glm::mat3 value;

        glm::mat3& init();
    };

}