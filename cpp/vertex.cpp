#include <vertex.h>

namespace gl {

    vertex_format vertex_default::format = {
            { vec3 },
            sizeof(vertex_default)
    };

    vertex_format vertex_solid::format = {
            { vec3, vec4 },
            sizeof(vertex_solid)
    };

    vertex_format vertex_solid_normal::format = {
            { vec3, vec4, vec3 },
            sizeof(vertex_solid_normal)
    };

    vertex_format vertex_uv::format = {
            { vec3, vec2 },
            sizeof(vertex_uv)
    };

    vertex_format vertex_uv_normal::format = {
            { vec3, vec2, vec3 },
            sizeof(vertex_uv_normal)
    };

    vertex_format vertex_tbn::format = {
            { vec3, vec2, vec3, vec3 },
            sizeof(vertex_tbn)
    };

}