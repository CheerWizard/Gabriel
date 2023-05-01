#include <vertex.h>

namespace gl {

    VertexFormat VertexDefault::format = {
            { attr::pos3 },
            sizeof(VertexDefault)
    };

    VertexFormat Vertex2dUV::format = {
            { attr::pos2, attr::uv },
            sizeof(Vertex2dUV)
    };

    VertexFormat VertexUV::format = {
            { attr::pos3, attr::uv },
            sizeof(VertexUV)
    };

    VertexFormat VertexTBN::format = {
            { attr::pos3, attr::uv, attr::normal, attr::tangent },
            sizeof(VertexTBN)
    };

}