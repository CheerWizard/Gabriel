#include <debugging/polygon_visual.h>

namespace gl {

    PolygonVisualRenderer::PolygonVisualRenderer() {
        mShader.addVertexStage("shaders/polygon_visual.vert");
        mShader.addFragmentStage("shaders/polygon_visual.frag");
        mShader.complete();
    }

    PolygonVisualRenderer::~PolygonVisualRenderer() {
        mShader.free();
    }

    void PolygonVisualRenderer::begin() {
        polygonLine();
        mShader.use();
    }

    void PolygonVisualRenderer::end() {
        polygonFill();
    }

    void PolygonVisualRenderer::render(PolygonVisual& polygonVisual, Transform& transform, DrawableElements& drawable) {
        mShader.setUniformArgs("color", polygonVisual.color);
        mShader.setUniformArgs("thickness", polygonVisual.thickness);
        transform.update(mShader);
        drawable.draw();
    }

}