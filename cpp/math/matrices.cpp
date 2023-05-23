
namespace gl {

    glm::mat4& OrthoMat::init() {
        value = glm::ortho(left, right, bottom, top, zNear, zFar);
        return value;
    }

    glm::mat4& PerspectiveMat::init() {
        value = glm::perspective(glm::radians(fovDegree), aspectRatio, zNear, zFar);
        return value;
    }

    glm::mat4& ViewMat::init() {
        value = glm::lookAt(eye, center, up);
        return value;
    }

    glm::mat4& ModelMat::init() {
        glm::mat4 result(1.0f);
        result = glm::translate(result, translation);
        result = glm::rotate(result, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        result = glm::rotate(result, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        result = glm::rotate(result, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        result = glm::scale(result, scale);
        value = result;
        return value;
    }

    glm::mat4& RigidBody::init() {
        glm::mat4 result(1.0f);
        result = glm::translate(result, translation);
        result = glm::rotate(result, rotation.x, glm::vec3(1, 0, 0));
        result = glm::rotate(result, rotation.x, glm::vec3(0, 1, 0));
        result = glm::rotate(result, rotation.x, glm::vec3(0, 0, 1));
        value = result;
        return value;
    }

}
