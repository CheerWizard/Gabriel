
#include <math/matrices.h>

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
        value = glm::mat4(1.0f);
        value = glm::translate(value, translation);
        value = glm::rotate(value, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        value = glm::rotate(value, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        value = glm::rotate(value, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        value = glm::scale(value, scale);
        return value;
    }

    glm::mat4& RigidBody::init() {
        value = glm::mat4(1.0f);
        value = glm::translate(value, translation);
        value = glm::rotate(value, rotation.x, glm::vec3(1, 0, 0));
        value = glm::rotate(value, rotation.x, glm::vec3(0, 1, 0));
        value = glm::rotate(value, rotation.x, glm::vec3(0, 0, 1));
        return value;
    }

    glm::mat3& Model2dMat::init() {
        glm::mat3 T = glm::mat3(1.0f);
        T[2] = { translation.x, translation.y, 1 };

        glm::mat3 R = glm::mat3(1.0f);
        float radians = glm::radians(rotation);
        float sin = glm::sin(radians);
        float cos = glm::cos(radians);
        R[0] = { cos, sin, 0 };
        R[1] = { -sin, cos, 0 };

        glm::mat3 S = glm::mat3(1.0f);
        S[0][0] = scale.x;
        S[1][1] = scale.y;

        value = T * R;
        value = S * value;

        return value;
    }
}
