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
        T* v0 = &vertices.v0;
        T* v1 = &vertices.v1;
        T* v2 = &vertices.v2;
        T* v3 = &vertices.v3;

        glm::vec3& pos0 = v0->pos;
        glm::vec3& pos1 = v1->pos;
        glm::vec3& pos2 = v2->pos;

        glm::vec2& uv0 = v0->uv;
        glm::vec2& uv1 = v1->uv;
        glm::vec2& uv2 = v2->uv;

        // Calculate triangle edges
        glm::vec3 dpos_1 = pos1 - pos0;
        glm::vec3 dpos_2 = pos2 - pos0;

        // Calculate delta UV
        glm::vec2 duv_1 = uv1 - uv0;
        glm::vec2 duv_2 = uv2 - uv0;

        // Calculate tangent and bitangent with formula
        float r = 1.0f / (duv_1.x * duv_2.y - duv_1.y * duv_2.x);

        glm::vec3 tangent = (dpos_1 * duv_2.y - dpos_2 * duv_1.y) * r;
        v0->tangent = tangent;
        v1->tangent = tangent;
        v2->tangent = tangent;
        v3->tangent = tangent;

        glm::vec3 bitangent = (dpos_2 * duv_1.x - dpos_1 * duv_2.x) * r;
        v0->bitangent = bitangent;
        v1->bitangent = bitangent;
        v2->bitangent = bitangent;
        v3->bitangent = bitangent;
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