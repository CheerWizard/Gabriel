#include <terrain.h>

namespace gl {

    void Terrain::init() {
        plane.init(*drawable());
    }

    void Terrain::free() {
        plane.free();
    }

    void Terrain::displace_with(const DisplacementMap& displacementMap, float scale) {
        displacement()->set_origin_vertices(&plane.vertices);
        displacement()->scale = scale;
        displacement()->map = displacementMap;
        displacement()->displace(*drawable());
    }

    void Terrain::displace(float scale) {
        displacement()->set_origin_vertices(&plane.vertices);
        displacement()->scale = scale;
        displacement()->displace(*drawable());
    }

}