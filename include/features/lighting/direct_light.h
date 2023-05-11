#pragma once

#include <ecs/entity.h>

namespace gl {

    component(DirectLightComponent) {
        glm::vec3 direction = { 0, 0, 0 };
        glm::vec4 position = { 5, 5, 5, 0 };
        glm::vec4 color = { 1, 1, 1, 1 };
    };

    struct DirectLight : Entity {

        DirectLight() : Entity() {}

        DirectLight(Scene* scene) : Entity(scene) {
            addComponent<DirectLightComponent>();
        }

        DirectLightComponent& value();

    };

}