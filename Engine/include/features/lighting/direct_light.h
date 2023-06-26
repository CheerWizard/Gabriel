#pragma once

#include <features/lighting/light_color.h>

namespace gl {

    component(DirectLightComponent) {
        glm::vec3 direction = { 0, 0, 0 };
        glm::vec4 position = { 5, 5, 5, 0 };
        LightColor color = { 1, 1, 1, 1 };
    };

    struct GABRIEL_API DirectLight : Entity {

        DirectLight() : Entity() {}

        DirectLight(Scene* scene) : Entity(scene) {
            addComponent<DirectLightComponent>();
        }

        DirectLightComponent& value();

    };

}