#include <vertex.h>

namespace gl {

    VertexFormat VertexDefault::format = {
            { vec3 },
            sizeof(VertexDefault)
    };

    VertexFormat VertexSolid::format = {
            { vec3, vec4 },
            sizeof(VertexSolid)
    };

    VertexFormat VertexSolidNormal::format = {
            { vec3, vec4, vec3 },
            sizeof(VertexSolidNormal)
    };

    VertexFormat VertexUV::format = {
            { vec3, vec2 },
            sizeof(VertexUV)
    };

    VertexFormat VertexUVNormal::format = {
            { vec3, vec2, vec3 },
            sizeof(VertexUVNormal)
    };

    VertexFormat VertexTBN::format = {
            { vec3, vec2, vec3, vec3 },
            sizeof(VertexTBN)
    };

}