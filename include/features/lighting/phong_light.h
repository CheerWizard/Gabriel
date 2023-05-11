#pragma once

#include <ecs/entity.h>

namespace gl {

    component(PhongLightComponent) {
        glm::vec3 padding0 = { 0, 0, 0 };
        glm::vec4 position = { 0, 0, 0, 1 };
        glm::vec4 color = { 1, 1, 1, 1 };
    };

    struct PhongLight : Entity {

        PhongLight() : Entity() {}

        PhongLight(Scene* scene) : Entity(scene) {
            addComponent<PhongLightComponent>();
        }

        PhongLightComponent& value();

    };

}