#include <plane.h>

namespace gl {

    PlaneUV::PlaneUV() : Plane<VertexUV>() {}

    PlaneUV::PlaneUV(int size) : Plane(size) {}

    PlaneTBN::PlaneTBN() : Plane<VertexTBN>() {
        generate_tbn();
    }

    PlaneTBN::PlaneTBN(int size) : Plane<VertexTBN>(size) {
        generate_tbn();
    }

    void PlaneTBN::generate_tbn() {
        for (int i = 0 ; i < vertices.count ; i++) {
            auto& V = vertices[i];
            V.normal = { 0, 1, 0 };
            V.tangent = { 0, 0, 1 };
        }
    }

}