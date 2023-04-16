#include <plane.h>

namespace gl {

    void PlaneTBN::generate_tbn() {
        for (int i = 0 ; i < vertices.count ; i++) {
            auto& V = vertices[i];
            V.normal = { 0, 1, 0 };
            V.tangent = { 0, 0, 1 };
        }
    }

}