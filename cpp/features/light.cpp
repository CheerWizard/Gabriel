#include <features/light.h>

namespace gl {

    PhongLightUniform& PhongLight::value() {
        return getComponent<PhongLightComponent>()->value;
    }

    DirectLightUniform& DirectLight::value() {
        return getComponent<DirectLightComponent>()->value;
    }

    glm::vec4& DirectLight::direction() {
        return getComponent<DirectLightComponent>()->direction;
    }

    PointLightUniform& PointLight::value() {
        return getComponent<PointLightComponent>()->value;
    }

    SpotLightUniform& SpotLight::value() {
        return getComponent<SpotLightComponent>()->value;
    }

}