#include <features/outline.h>

namespace gl {

    OutlineRenderer::OutlineRenderer() {
        mShader.addVertexStage("shaders/outline.vert");
        mShader.addFragmentStage("shaders/outline.frag");
        mShader.complete();
    }

    OutlineRenderer::~OutlineRenderer() {
        mShader.free();
    }

    void OutlineRenderer::bind() {
        glStencilMask(GL_FALSE);
        glStencilFunc(GL_NOTEQUAL, 1, GL_TRUE);
        glDisable(GL_DEPTH_TEST);
    }

    void OutlineRenderer::unbind() {
        glStencilMask(GL_TRUE);
        glStencilFunc(GL_ALWAYS, 1, GL_TRUE);
        glEnable(GL_DEPTH_TEST);
    }

    void OutlineRenderer::use() {
        mShader.use();
    }

    void OutlineRenderer::render(Outline& outline, Transform& transform, DrawableElements& drawable) {
        mShader.setUniformArgs<float>("outline_thickness", outline.thickness);
        mShader.setUniformArgs<glm::vec4>("outline_color", outline.color);
        transform.update(mShader);
        drawable.draw();
    }

}