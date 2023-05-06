#pragma once

#include <api/shader.h>
#include <api/draw.h>

#include <features/transform.h>
#include <features/material.h>

namespace gl {

    struct SkeletalRenderer {

        ~SkeletalRenderer();

        void begin();

        void setCameraPos(glm::vec3& cameraPos);
        void updateBones(std::vector<glm::mat4>& bones);

        void render(EntityID entityId, Transform& transform, DrawableElements& drawable);
        void render(EntityID entityId, Transform& transform, DrawableElements& drawable, Material& material);

    protected:
        Shader mShader;
    };

}