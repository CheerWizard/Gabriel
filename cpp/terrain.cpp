#include <terrain.h>

namespace gl {

    void Terrain::init() {
        plane.init(*drawable());
    }

    void Terrain::free() {
        plane.free();
    }

    void Terrain::displace(float scale, int iterations, float min_height, float max_height, float filter) {
        displacement()->set_origin_vertices(&plane.vertices);
        displacement()->scale = scale;
        displacement()->map = FaultFormation(plane.size, plane.size, iterations, min_height, max_height, filter);
        displacement()->displace(*drawable());
    }

    void Terrain::displace(float scale, const Image &image) {
        displacement()->set_origin_vertices(&plane.vertices);
        displacement()->scale = scale;
        displacement()->map = HeightMap(image);
        displacement()->displace(*drawable());
    }

}