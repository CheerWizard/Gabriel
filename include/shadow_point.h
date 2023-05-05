#pragma once

#include <shader.h>
#include <frame.h>
#include <transform.h>
#include <draw.h>
#include <camera.h>

namespace gl {

    struct PointShadow : PerspectiveMat {
        Camera* camera = null;
        DepthAttachment map;

        std::vector<glm::mat4> update(const glm::vec3& lightPosition);
    };

    struct PointShadowRenderer final {

        PointShadowRenderer();
        ~PointShadowRenderer();

        void begin();
        void end();

        void render(Transform& transform, DrawableElements& drawable, PointShadow& pointShadow, const glm::vec3& lightPosition);

    private:
        Shader mShader;
    };

}