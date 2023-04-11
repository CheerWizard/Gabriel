#pragma once

#include <plane.h>
#include <displacement.h>
#include <entity.h>
#include <material.h>
#include <transform.h>

namespace gl {

    struct Terrain : ecs::Entity {
        PlaneTBN plane;

        Terrain() = default;

        Terrain(ecs::Scene* scene, int size)
        : Entity(scene) {
            plane.size = size;
            add_component<Transform>();
            add_component<DrawableElements>();
            add_component<Material>();
            add_component<DisplacementTBN>();
        }

        Terrain(ecs::Scene* scene, const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale, int size)
        : Entity(scene) {
            plane.size = size;
            add_component<Transform>(translation, rotation, scale);
            add_component<DrawableElements>();
            add_component<Material>();
            add_component<DisplacementTBN>();
        }

        Transform* transform() {
            return scene->get_component<Transform>(id);
        }

        DrawableElements* drawable() {
            return scene->get_component<DrawableElements>(id);
        }

        Material* material() {
            return scene->get_component<Material>(id);
        }

        DisplacementTBN* displacement() {
            return scene->get_component<DisplacementTBN>(id);
        }

        void init();
        void free();

        void displace(float scale, const Image& image);
        void displace(float scale, int iterations, float min_height, float max_height, float filter);
    };

}