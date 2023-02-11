#include <vertex.h>

namespace gl {

    vertex_format vertex_solid::format = {
            { vec3, vec4 },
            sizeof(vertex_solid)
    };

    shader_props vertex_solid::shader_props = {
            "shaders/solid.vert",
            "shaders/solid.frag"
    };

    vertex_format vertex_solid_normal::format = {
            { vec3, vec4, vec3 },
            sizeof(vertex_solid_normal)
    };

    shader_props vertex_solid_normal::shader_props = {
            "shaders/solid_normal.vert",
            "shaders/solid_normal.frag"
    };

    vertex_format vertex_uv::format = {
            { vec3, vec2 },
            sizeof(vertex_uv)
    };

    shader_props vertex_uv::shader_props = {
            "shaders/uv.vert",
            "shaders/uv.frag"
    };

    vertex_format vertex_uv_normal::format = {
            { vec3, vec2, vec3 },
            sizeof(vertex_uv_normal)
    };

    shader_props vertex_uv_normal::shader_props = {
            "shaders/uv_normal.vert",
            "shaders/uv_normal.frag"
    };

}