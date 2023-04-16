#include <sphere.h>

namespace gl {

    void SphereTBN::generate_tbn() {
        for (int i = 0 ; i < vertices.count ; i++) {
            auto& V = vertices[i];
            V.normal = V.pos;
            glm::vec3 right = { 1, 0, 0 };
            glm::vec3 up = { 0, 1, 0 };

            if (glm::dot(up, V.normal) == 1) {
                V.tangent = right;
            } else {
                V.tangent = glm::normalize(glm::cross(up, V.normal));
            }
        }
    }

}