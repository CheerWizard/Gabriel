#pragma once

#include <cube.h>
#include <transform.h>
#include <camera.h>
#include <frame.h>
#include <entity.h>

namespace gl {

    struct LightPresent final {
        u32 id = 0;
        glm::vec4 color = { 1, 1, 1, 1 };
        Transform transform = {
                { 0, 0, -2 },
                { 0, 0, 0 },
                { 0.1, 0.1, 0.1 },
        };
        CubeDefault presentation;

        void init();
        void free();

        void update();
    };

    struct PhongLightUniform final {
        glm::vec4 position = { 0, 0, 0, 1 };
        glm::vec4 color = { 1, 1, 1, 1 };
    };

    component(PhongLightComponent) {
        PhongLightUniform value;
    };

    struct PhongLight : ecs::Entity {

        PhongLight() : ecs::Entity() {}

        PhongLight(ecs::Scene* scene) : ecs::Entity(scene) {
            add_component<PhongLightComponent>();
        }

        PhongLightUniform& value();

    };

    struct DirectLightUniform final {
        glm::vec4 direction = { -2.0f, 4.0f, -1.0f, 0 };
        glm::vec4 color = { 1, 1, 1, 1 };
    };

    component(DirectLightComponent) {
        DirectLightUniform value;
    };

    struct DirectLight : ecs::Entity {

        DirectLight() : ecs::Entity() {}

        DirectLight(ecs::Scene* scene) : ecs::Entity(scene) {
            add_component<DirectLightComponent>();
        }

        DirectLightUniform& value();

    };

    struct PointLightUniform final {
        glm::vec4 position = { 0, 0, 0, 0 };
        glm::vec4 color = { 1, 1, 1, 1 };
        float constant = 0;
        float linear = 0;
        float quadratic = 1;
        float refraction = 1;
    };

    component(PointLightComponent) {
        PointLightUniform value;
    };

    struct PointLight : ecs::Entity {

        PointLight() : ecs::Entity() {}

        PointLight(ecs::Scene* scene) : ecs::Entity(scene) {
            add_component<PointLightComponent>();
        }

        PointLightUniform& value();

    };

    struct SpotLightUniform final {
        glm::vec4 position = { 0, 0, 0, 0 };
        glm::vec4 direction = { -0.2f, -1.0f, -0.3f, 0 };
        glm::vec4 color = { 1, 1, 1, 1 };
        float cutoff = glm::cos(glm::radians(6.5f));
        float outer = glm::cos(glm::radians(11.5f));
        float refraction = 1;
        float pad = 0;
    };

    component(SpotLightComponent) {
        SpotLightUniform value;
    };

    struct SpotLight : ecs::Entity {

        SpotLight() : ecs::Entity() {}

        SpotLight(ecs::Scene* scene) : ecs::Entity(scene) {
            add_component<SpotLightComponent>();
        }

        SpotLightUniform& value();

    };

}