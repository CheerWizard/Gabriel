#pragma once

#include <shader.h>
#include <matrices.h>
#include <frame.h>
#include <transform.h>
#include <draw.h>

namespace gl {

    struct PointShadow : PerspectiveMat {
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