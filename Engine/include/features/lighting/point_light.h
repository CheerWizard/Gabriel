#pragma once

#include <features/lighting/light_color.h>

namespace gl {

    component(PointLightComponent) {
        glm::vec3 padding0 = { 0, 0, 0 };
        glm::vec4 position = { 0, 0, 0, 0 };
        LightColor color = { 1, 1, 1, 1 };
        float constant = 0;
        float linear = 0;
        float quadratic = 1;
        float refraction = 1;
    };

    struct GABRIEL_API PointLight : Entity {

        PointLight() : Entity() {}

        PointLight(Scene* scene) : Entity(scene) {
            addComponent<PointLightComponent>();
        }

        PointLightComponent& value();

    };

}