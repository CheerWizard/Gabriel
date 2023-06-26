#include <features/outline.h>

namespace gl {

    void OutlineShader::init() {
        addVertexStage("shaders/outline.vert");
        addFragmentStage("shaders/outline.frag");
        complete();
    }

    void OutlineShader::update(Outline& outline) {
        params.color.value = outline.color;
        params.thickness.value = outline.thickness;

        setUniform(params.color);
        setUniform(params.thickness);
    }

    OutlineRenderer::OutlineRenderer() {
        mShader.init();
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
        mShader.update(outline);
        transform.update(mShader);
        drawable.draw();
    }

}