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
            plane = PlaneTBN(size);
            add_component<Transform>();
            add_component<DrawableElements>();
            add_component<Material>();
            add_component<DisplacementTBN>();
        }

        Terrain(ecs::Scene* scene, const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale, int size)
        : Entity(scene) {
            plane = PlaneTBN(size);
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

        inline int size() const { return plane.size; }

        void init();
        void free();

        void displace_with(const DisplacementMap& displacement_map, float scale);
        void displace(float scale);
    };

}