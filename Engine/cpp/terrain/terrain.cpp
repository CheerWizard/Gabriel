#include <terrain/terrain.h>

namespace gl {

    void TerrainBuilder::init() {
        plane.init(terrain.drawable);
    }

    void TerrainBuilder::free() {
        plane.free();
    }

    void TerrainBuilder::displaceWith(const DisplacementMap& displacementMap, float scale) {
        displacement.setOriginVertices(&plane.vertices);
        displacement.scale = scale;
        displacement.map = displacementMap;
        displacement.displace(terrain.drawable);
    }

    void TerrainBuilder::displace(float scale) {
        displacement.setOriginVertices(&plane.vertices);
        displacement.scale = scale;
        displacement.displace(terrain.drawable);
    }

}