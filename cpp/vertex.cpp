#include <vertex.h>

namespace gl {

    VertexFormat VertexDefault::format = {
            { attr::pos },
            sizeof(VertexDefault)
    };

    VertexFormat VertexUV::format = {
            { attr::pos, attr::uv },
            sizeof(VertexUV)
    };

    VertexFormat VertexTBN::format = {
            { attr::pos, attr::uv, attr::normal, attr::tangent },
            sizeof(VertexTBN)
    };

}