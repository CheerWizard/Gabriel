#include <shadow_direct.h>

namespace gl {

    glm::mat4 DirectShadow::update(const glm::vec3& lightDirection) {
        glm::mat4 lightProjection = init();
        glm::mat4 lightView = ViewMat {
                lightDirection,
                glm::vec3(0.0f, 0.0f,  0.0f),
                glm::vec3(0.0f, 1.0f,  0.0f)
        }.init();
        glm::mat4 lightSpace = lightProjection * lightView;
        lightSpaces.emplace_back(lightSpace);
        return lightSpace;
    }

    DirectShadowRenderer::DirectShadowRenderer() {
        mShader.add_vertex_stage("shaders/direct_shadow.vert");
        mShader.add_fragment_stage("shaders/direct_shadow.frag");
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
            const glm::vec3& lightDirection
    ) {
        glm::mat4 lightSpace = directShadow.update(lightDirection);
        mShader.set_uniform_args("direct_light_space", lightSpace);
        transform.update(mShader);
        drawable.draw();
    }

}