#pragma once

#include <api/frame.h>

#include <ecs/entity.h>

#include <geometry/cube.h>

#include <features/transform.h>

#include <control/camera.h>

namespace gl {

    struct PhongLightUniform final {
        glm::vec4 position = { 0, 0, 0, 1 };
        glm::vec4 color = { 1, 1, 1, 1 };
    };

    component(PhongLightComponent) {
        PhongLightUniform value;
    };

    struct PhongLight : Entity {

        PhongLight() : Entity() {}

        PhongLight(Scene* scene) : Entity(scene) {
            addComponent<PhongLightComponent>();
        }

        PhongLightUniform& value();

    };

    struct DirectLightUniform final {
        glm::vec4 position = { 5, 5, 5, 0 };
        glm::vec4 color = { 1, 1, 1, 1 };
    };

    component(DirectLightComponent) {
        DirectLightUniform value;
        glm::vec4 direction = { 0, 0, 0, 0 };
    };

    struct DirectLight : Entity {

        DirectLight() : Entity() {}

        DirectLight(Scene* scene) : Entity(scene) {
            addComponent<DirectLightComponent>();
        }

        DirectLightUniform& value();
        glm::vec4& direction();

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

    struct PointLight : Entity {

        PointLight() : Entity() {}

        PointLight(Scene* scene) : Entity(scene) {
            addComponent<PointLightComponent>();
        }

        PointLightUniform& value();

    };

    struct SpotLightUniform final {
        glm::vec4 position = { 0, 0, 0, 0 };
        glm::vec4 direction = { -0.2f, -1.0f, -0.3f, 0 };
        glm::vec4 color = { 1, 1, 1, 1 };
        float cutoff = 6.5f;
        float outer = 11.5f;
        float refraction = 1;
        float pad = 0;

        inline void setCutOff(float cutoff) {
            this->cutoff = glm::cos(glm::radians(cutoff));
        }

        inline void setOuter(float outer) {
            this->outer = glm::cos(glm::radians(outer));
        }
    };

    component(SpotLightComponent) {
        SpotLightUniform value;
    };

    struct SpotLight : Entity {

        SpotLight() : Entity() {}

        SpotLight(Scene* scene) : Entity(scene) {
            addComponent<SpotLightComponent>();
        }

        SpotLightUniform& value();

    };

}