#include <features/shadow/shadow_direct.h>

namespace gl {

    glm::mat4 DirectShadow::update(const glm::vec3& lightPos, const glm::vec3& lightDirection) {
        glm::mat4 lightProjection = init();
        glm::mat4 lightView = ViewMat { lightPos, lightDirection, { 0, 0, 1 } }.init();
        glm::mat4 lightSpace = lightProjection * lightView;
        lightSpaces.emplace_back(lightSpace);
        return lightSpace;
    }

    void DirectShadow::update(const glm::vec3& lightDirection, glm::mat4 &lightProjection, glm::mat4 &lightView) {
        // create frustum
        PerspectiveMat cameraPerspective = camera->getPerspectiveMat();
        ViewMat cameraView = camera->getViewMat();
        Frustum frustum(cameraPerspective);

        // transform frustum into world space
        frustum.toWorldSpace(cameraView);
        Frustum frustumWorldSpace = frustum;

        // transform frustum into light space
        glm::vec3 lightOrigin = { 0, 0, 0 };
        glm::vec3 lightUp = { 0, 1, 0 };
        lightView = ViewMat { lightOrigin, -lightDirection, lightUp }.init();
        frustum * lightView;

        // calculate AABB for view frustum in light space
        AABB aabb;
        frustum.fill(aabb);

        // calculate light position in world space
        glm::vec3 bottomLeft = { aabb.minX, aabb.minY, aabb.minZ };
        glm::vec3 topRight = { aabb.maxX, aabb.maxY, aabb.minZ };
        glm::vec4 lightPosition = glm::vec4((bottomLeft + topRight) / 2.0f, 1.0f);
        lightPosition = glm::inverse(lightView) * lightPosition;
        glm::vec3 lightPos = lightPosition;

        // transform world frustum into light space
        lightView = ViewMat { lightPos, -lightDirection, lightUp }.init();
        frustumWorldSpace * lightView;

        // recalculate AABB
        aabb = {};
        frustumWorldSpace.fill(aabb);
        aabb.update(*this);

        lightProjection = init();
    }

    DirectShadowRenderer::DirectShadowRenderer() {
        mShader.addVertexStage("shaders/direct_shadow.vert");
        mShader.addFragmentStage("shaders/direct_shadow.frag");
        mShader.complete();
    }

    DirectShadowRenderer::~DirectShadowRenderer() {
        mShader.free();
    }

    void DirectShadowRenderer::begin() {
        mShader.use();
    }

    void DirectShadowRenderer::end() {
        Shader::stop();
    }

    void DirectShadowRenderer::render(
            Transform& transform,
            DrawableElements& drawable,
            DirectShadow& directShadow,
            const glm::vec3& lightPos,
            const glm::vec3& lightDirection
    ) {
        glm::mat4 lightSpace = directShadow.update(lightPos, lightDirection);
        mShader.setUniformArgs("direct_light_space", lightSpace);
        transform.update(mShader);
        drawable.draw();
    }

}