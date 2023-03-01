#include <rect.h>

namespace gl {

    template<typename T>
    static void init_uv(rect_vertices<T>& vertices) {
        vertices.v0.uv = { 0, 0 };
        vertices.v1.uv = { 0, 1 };
        vertices.v2.uv = { 1, 1 };
        vertices.v3.uv = { 1, 0 };
    }

    template<typename T>
    static void init_normal(rect_vertices<T>& vertices) {
        glm::vec3 normal = -glm::normalize(glm::cross(
                vertices.v1.pos - vertices.v0.pos,
                vertices.v3.pos - vertices.v0.pos
        ));
        vertices.v0.normal = normal;
        vertices.v1.normal = normal;
        vertices.v2.normal = normal;
        vertices.v3.normal = normal;
    }

    template<typename T>
    static void init_tbn(rect_vertices<T>& vertices) {
        init_tbn(&vertices.v0, &vertices.v1, &vertices.v2, &vertices.v3);
    }

    void rect_uv_init(drawable_elements& drawable) {
        rect_uv rect;
        init_uv(rect.vertices);
        rect_init(drawable, rect);
    }

    void rect_uv_normal_init(drawable_elements& drawable) {
        rect_uv_normal rect;
        init_uv(rect.vertices);
        init_normal(rect.vertices);
        rect_init(drawable, rect);
    }

    void rect_tbn_init(drawable_elements& drawable) {
        rect_tbn rect;
        init_uv(rect.vertices);
        init_normal(rect.vertices);
        init_tbn(rect.vertices);
        rect_init(drawable, rect);
    }

}