#pragma once

#include <features/lighting/light_color.h>

namespace gl {

    component(PhongLightComponent) {
        glm::vec3 padding0 = { 0, 0, 0 };
        glm::vec4 position = { 0, 0, 0, 1 };
        LightColor color = { 1.0f, 1.0f, 1.0f, 1.0f };
    };

    struct GABRIEL_API PhongLight : Entity {

        PhongLight() : Entity() {}

        PhongLight(Scene* scene) : Entity(scene) {
            addComponent<PhongLightComponent>();
        }

        PhongLightComponent& value();

    };

}