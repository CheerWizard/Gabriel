#include <terrain.h>

namespace gl {

    void TerrainBuilder::init() {
        plane.init(terrain.drawable);
    }

    void TerrainBuilder::free() {
        Entity::free();
        plane.free();
    }

    void TerrainBuilder::displace_with(const DisplacementMap& displacementMap, float scale) {
        displacement.set_origin_vertices(&plane.vertices);
        displacement.scale = scale;
        displacement.map = displacementMap;
        displacement.displace(terrain.drawable);
    }

    void TerrainBuilder::displace(float scale) {
        displacement.set_origin_vertices(&plane.vertices);
        displacement.scale = scale;
        displacement.displace(terrain.drawable);
    }

}