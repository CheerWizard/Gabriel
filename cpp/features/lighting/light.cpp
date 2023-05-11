#include <features/lighting/light.h>

namespace gl {

    Scene* LightStorage::scene = null;

    PhongLightBuffer LightStorage::phongLightBuffer = 1;
    DirectLightBuffer LightStorage::directLightBuffer = 1;
    PointLightBuffer LightStorage::pointLightBuffer = 4;
    SpotLightBuffer LightStorage::spotLightBuffer = 1;

    void LightStorage::init() {
        phongLightBuffer.init(1);
        directLightBuffer.init(2);
        pointLightBuffer.init(3);
        spotLightBuffer.init(4);
    }

    void LightStorage::free() {
        phongLightBuffer.free();
        directLightBuffer.free();
        pointLightBuffer.free();
        spotLightBuffer.free();
    }

    void LightStorage::update() {
        phongLightBuffer.update(scene);
        directLightBuffer.update(scene);
        pointLightBuffer.update(scene);
        spotLightBuffer.update(scene);
    }

}