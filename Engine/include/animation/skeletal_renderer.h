#pragma once

#include <api/draw.h>

#include <features/transform.h>
#include <features/material.h>

namespace gl {

    struct GABRIEL_API SkeletalRenderer {

        ~SkeletalRenderer();

        void use();

        void updateBones(std::vector<glm::mat4>& bones);

        void render(Transform& transform, DrawableElements& drawable);
        void render(Transform& transform, DrawableElements& drawable, Material& material);

    protected:
        Shader mShader;
    };

}