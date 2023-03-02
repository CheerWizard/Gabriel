#pragma once

#include <triangle.h>
#include <rect.h>
#include <cube.h>

namespace gl {

    void free(u32 shader_program, u32 vao, u32 vbo);
    void free(u32 shader_program, u32 vao, u32 vbo, u32 ibo);

    template<typename T>
    static void init_tbn(T* v0, T* v1, T* v2, T* v3) {
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

        // Calculate tangent
        float r = 1.0f / (duv_1.x * duv_2.y - duv_1.y * duv_2.x);
        glm::vec3 tangent = (dpos_1 * duv_2.y - dpos_2 * duv_1.y) * r;
        v0->tangent = tangent;
        v1->tangent = tangent;
        v2->tangent = tangent;
        v3->tangent = tangent;
    }

}
