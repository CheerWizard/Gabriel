#include <sphere.h>

namespace gl {

    SphereUV::SphereUV() : Sphere<VertexUV>() {}

    SphereUV::SphereUV(int x_segments, int y_segments) : Sphere<VertexUV>(x_segments, y_segments) {}

    SphereTBN::SphereTBN() : Sphere<VertexTBN>() {
        generate_tbn();
    }

    SphereTBN::SphereTBN(int x_segments, int y_segments) : Sphere<VertexTBN>(x_segments, y_segments) {
        generate_tbn();
    }

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