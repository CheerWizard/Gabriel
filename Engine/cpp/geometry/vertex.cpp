#include <geometry/vertex.h>

namespace gl {

    VertexFormat VertexDefault::format = {
            { Attributes::POS3 },
            sizeof(VertexDefault)
    };

    VertexFormat Vertex2dUV::format = {
            { Attributes::POS2, Attributes::UV },
            sizeof(Vertex2dUV)
    };

    VertexFormat VertexUV::format = {
            { Attributes::POS3, Attributes::UV },
            sizeof(VertexUV)
    };

    VertexFormat VertexTBN::format = {
            { Attributes::POS3, Attributes::UV, Attributes::NORMAL, Attributes::TANGENT },
            sizeof(VertexTBN)
    };

}