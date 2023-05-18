#pragma once

#include <api/frame.h>
#include <api/draw.h>

#include <features/transform.h>

#include <control/camera.h>

namespace gl {

    struct DirectShadow : OrthoMat {
        Camera* camera = null;
        DepthAttachment map;
        std::vector<glm::mat4> lightSpaces;
        int filterSize = 9;

        glm::mat4 update(const glm::vec3& lightPos, const glm::vec3& lightDirection);

    private:
        void update(const glm::vec3& lightDirection, glm::mat4& lightProjection, glm::mat4& lightView);
    };

    struct DirectShadowRenderer final {

        DirectShadowRenderer();
        ~DirectShadowRenderer();

        void begin();
        void end();

        void render(
                Transform& transform,
                DrawableElements& drawable,
                DirectShadow& directShadow,
                const glm::vec3& lightPos,
                const glm::vec3& lightDirection
        );

    private:
        Shader mShader;
    };

}