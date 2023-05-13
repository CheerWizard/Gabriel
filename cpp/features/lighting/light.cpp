#include <features/lighting/light.h>

#include <math/maths.h>

namespace gl {

    Scene* LightStorage::scene = null;

    PhongLightBuffer LightStorage::phongLightBuffer = 1;
    DirectLightBuffer LightStorage::directLightBuffer = 1;
    PointLightBuffer LightStorage::pointLightBuffer = 1;
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

    float LightMath::radius(const glm::vec3& color, const float brightness, const float quadratic, const float linear, const float constant) {
        float Imax  = std::fmaxf(std::fmaxf(color.r, color.g), color.b);
        const float a = quadratic;
        const float b = linear;
        const float c = constant - (Imax * 256 / brightness);
        return (-b + sqrtf(b * b - 4 * a * c)) / (2 * a);
    }

}