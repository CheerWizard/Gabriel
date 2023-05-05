#pragma once

#include <shader.h>
#include <matrices.h>
#include <frame.h>
#include <transform.h>
#include <draw.h>

namespace gl {

    struct DirectShadow : OrthoMat {
        DepthAttachment map;
        std::vector<glm::mat4> lightSpaces;

        glm::mat4 update(const glm::vec3& lightDirection);
    };

    struct DirectShadowRenderer final {

        DirectShadowRenderer();
        ~DirectShadowRenderer();

        void begin();
        void end();

        void render(Transform& transform, DrawableElements& drawable, DirectShadow& directShadow, const glm::vec3& lightDirection);

    private:
        Shader mShader;
    };

}