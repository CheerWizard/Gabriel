#pragma once

#include <api/frame.h>
#include <api/draw.h>

#include <features/transform.h>

#include <control/camera.h>

namespace gl {

    struct GABRIEL_API PointShadow : PerspectiveMat {
        Camera* camera = null;
        DepthAttachment map;

        std::vector<glm::mat4> update(const glm::vec3& lightPosition);
    };

    struct GABRIEL_API PointShadowRenderer final {

        PointShadowRenderer();
        ~PointShadowRenderer();

        void begin();
        void end();

        void render(Transform& transform, DrawableElements& drawable, PointShadow& pointShadow, const glm::vec3& lightPosition);

    private:
        Shader mShader;
    };

}