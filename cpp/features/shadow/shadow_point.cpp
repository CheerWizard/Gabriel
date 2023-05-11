#include <features/shadow/shadow_point.h>

namespace gl {

    std::vector<glm::mat4> PointShadow::update(const glm::vec3& lightPosition) {
        glm::mat4 projection = init();
        return {
                projection * glm::lookAt(lightPosition, lightPosition + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
                projection * glm::lookAt(lightPosition, lightPosition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
                projection * glm::lookAt(lightPosition, lightPosition + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
                projection * glm::lookAt(lightPosition, lightPosition + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)),
                projection * glm::lookAt(lightPosition, lightPosition + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)),
                projection * glm::lookAt(lightPosition, lightPosition + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0))
        };
    }

    PointShadowRenderer::PointShadowRenderer() {
        mShader.addVertexStage("shaders/point_shadow.vert");
        mShader.addFragmentStage("shaders/point_shadow.frag");
        mShader.addGeometryStage("shaders/point_shadow.geom");
        mShader.complete();
    }

    PointShadowRenderer::~PointShadowRenderer() {
        mShader.free();
    }

    void PointShadowRenderer::begin() {
        mShader.use();
    }

    void PointShadowRenderer::end() {
        Shader::stop();
    }

    void PointShadowRenderer::render(
            Transform& transform,
            DrawableElements& drawable,
            PointShadow& pointShadow,
            const glm::vec3& lightPosition
    ) {
        std::vector<glm::mat4> lightSpaces = pointShadow.update(lightPosition);
        UniformArrayM4F lightSpacesUniform = { "light_spaces", lightSpaces };
        UniformF zFarUniform = { "far_plane", pointShadow.zFar };
        UniformV3F lightPositionUniform = { "light_pos", lightPosition };

        mShader.setUniformArray(lightSpacesUniform);
        mShader.setUniform(zFarUniform);
        mShader.setUniform(lightPositionUniform);
        transform.update(mShader);
        drawable.draw();
    }

}