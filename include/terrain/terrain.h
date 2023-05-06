#pragma once

#include <ecs/entity.h>

#include <geometry/plane.h>

#include <features/displacement.h>
#include <features/material.h>
#include <features/transform.h>

namespace gl {

    struct Terrain final {
        EntityID entityId;
        Transform transform;
        DrawableElements drawable;
        Material material;
    };

    struct TerrainBuilder : Entity {
        PlaneTBN plane;
        DisplacementTBN displacement;
        Terrain terrain;

        TerrainBuilder() : Entity() {}

        TerrainBuilder(Scene* scene, int size)
        : Entity(scene) {
            terrain.entityId = id;
            plane = PlaneTBN(size);
            init();
        }

        TerrainBuilder(Scene* scene, const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale, int size)
        : Entity(scene) {
            terrain.entityId = id;
            terrain.transform = { translation, rotation, scale };
            plane = PlaneTBN(size);
            init();
        }

        inline int size() const { return plane.size; }

        void free() override;

        void displaceWith(const DisplacementMap& displacementMap, float scale);
        void displace(float scale);

    private:
        void init();
    };

}