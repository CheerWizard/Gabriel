#include <sphere.h>

namespace gl {

    sphere_default sphere_default_init(drawable_elements& drawable, int x_segments, int y_segments) {
        sphere_default sphere = sphere_init<vertex_default>(drawable, [](vertex_default& V, const glm::vec3& pos, const glm::vec2& uv) {
            V.pos = pos;
        }, x_segments, y_segments);

        sphere_init(drawable, sphere);

        return sphere;
    }

    sphere_tbn sphere_tbn_init(drawable_elements& drawable, int x_segments, int y_segments) {
        sphere_tbn sphere = sphere_init<vertex_tbn>(drawable, [](vertex_tbn& V, const glm::vec3& pos, const glm::vec2& uv) {
            V.pos = pos;
            V.uv = uv;
            V.normal = pos;

            glm::vec3 right = { 1, 0, 0 };
            glm::vec3 up = { 0, 1, 0 };

            if (glm::dot(up, V.normal) == 1) {
                V.tangent = right;
            } else {
                V.tangent = glm::normalize(glm::cross(up, V.normal));
            }
        }, x_segments, y_segments);

        sphere_init(drawable, sphere);

        return sphere;
    }

    void sphere_tbn_displace(
            sphere_tbn& sphere,
            drawable_elements& drawable,
            const char* displacement_path,
            bool flip_uv,
            float scale,
            float shift
    ) {
        sphere_displace<vertex_tbn>(sphere, drawable, displacement_path, flip_uv, scale, shift, [](vertex_tbn& V) {
            V.normal = V.pos;

            glm::vec3 right = { 1, 0, 0 };
            glm::vec3 up = { 0, 1, 0 };

            if (glm::dot(up, V.normal) == 1) {
                V.tangent = right;
            } else {
                V.tangent = glm::normalize(glm::cross(up, V.normal));
            }
        });
    }

}