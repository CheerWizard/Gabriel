#pragma once

#include <features/lighting/light_color.h>

#include <ecs/entity.h>

namespace gl {

    component(SpotLightComponent) {
        float cutoff = 6.5f;
        float outer = 11.5f;
        float refraction = 1;
        glm::vec4 position = { 0, 0, 0, 0 };
        glm::vec4 direction = { -0.2f, -1.0f, -0.3f, 0 };
        LightColor color = { 1, 1, 1, 1 };

        inline void setCutOff(float cutoff) {
            this->cutoff = glm::cos(glm::radians(cutoff));
        }

        inline void setOuter(float outer) {
            this->outer = glm::cos(glm::radians(outer));
        }
    };

    struct SpotLight : Entity {

        SpotLight() : Entity() {}

        SpotLight(Scene* scene) : Entity(scene) {
            addComponent<SpotLightComponent>();
        }

        SpotLightComponent& value();

    };

}