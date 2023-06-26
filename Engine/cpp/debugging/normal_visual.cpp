#include <debugging/normal_visual.h>

namespace gl {

    NormalVisualRenderer::NormalVisualRenderer() {
        mShader.addVertexStage("shaders/normal_visual.vert");
        mShader.addFragmentStage("shaders/normal_visual.frag");
        mShader.addGeometryStage("shaders/normal_visual.geom");
        mShader.complete();
    }

    NormalVisualRenderer::~NormalVisualRenderer() {
        mShader.free();
    }

    void NormalVisualRenderer::begin() {
        mShader.use();
    }

    void NormalVisualRenderer::render(NormalVisual& normalVisual, Transform& transform, DrawableElements& drawable) {
        mShader.setUniformArgs("color", normalVisual.color);
        mShader.setUniformArgs("length", normalVisual.length);
        transform.update(mShader);
        drawable.draw();
    }

}