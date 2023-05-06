#include <math/matrices.h>

namespace gl {

    glm::mat4 OrthoMat::init() const {
        return glm::ortho(left, right, bottom, top, zNear, zFar);
    }

    glm::mat4 PerspectiveMat::init() const {
        return glm::perspective(glm::radians(fovDegree), aspectRatio, zNear, zFar);
    }

    glm::mat4 ViewMat::init() const {
        return glm::lookAt(eye, center, up);
    }

    glm::mat4 ModelMat::init() const {
        glm::mat4 result(1.0f);
        result = glm::translate(result, translation);
        result = glm::rotate(result, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        result = glm::rotate(result, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        result = glm::rotate(result, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        result = glm::scale(result, scale);
        return result;
    }
}