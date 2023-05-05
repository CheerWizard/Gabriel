#pragma once

#include <plane.h>
#include <displacement.h>
#include <entity.h>
#include <material.h>
#include <transform.h>

namespace gl {

    struct Terrain final {
        ecs::EntityID entityId;
        Transform transform;
        DrawableElements drawable;
        Material material;
    };

    struct TerrainBuilder : ecs::Entity {
        PlaneTBN plane;
        DisplacementTBN displacement;
        Terrain terrain;

        TerrainBuilder() : ecs::Entity() {}

        TerrainBuilder(ecs::Scene* scene, int size)
        : ecs::Entity(scene) {
            terrain.entityId = id;
            plane = PlaneTBN(size);
            init();
        }

        TerrainBuilder(ecs::Scene* scene, const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale, int size)
        : ecs::Entity(scene) {
            terrain.entityId = id;
            terrain.transform = { translation, rotation, scale };
            plane = PlaneTBN(size);
            init();
        }

        inline int size() const { return plane.size; }

        void free() override;

        void displace_with(const DisplacementMap& displacement_map, float scale);
        void displace(float scale);

    private:
        void init();
    };

}