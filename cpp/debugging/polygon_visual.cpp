#include <debugging/polygon_visual.h>

namespace gl {

    PolygonFace PolygonVisualRenderer::polygonFace = PolygonFace::FRONT;
    PolygonType PolygonVisualRenderer::polygonType = PolygonType::LINE;

    PolygonVisualRenderer::PolygonVisualRenderer() {
        mShader.addVertexStage("shaders/polygon_visual.vert");
        mShader.addFragmentStage("shaders/polygon_visual.frag");
        mShader.complete();
    }

    PolygonVisualRenderer::~PolygonVisualRenderer() {
        mShader.free();
    }

    void PolygonVisualRenderer::begin() {
        PolygonMode::set(polygonFace, polygonType);
        mShader.use();
    }

    void PolygonVisualRenderer::end() {
        PolygonMode::set(PolygonFace::FRONT, PolygonType::FILL);
    }

    void PolygonVisualRenderer::render(PolygonVisual& polygonVisual, Transform& transform, DrawableElements& drawable) {
        mShader.setUniformArgs("color", polygonVisual.color);
        mShader.setUniformArgs("thickness", polygonVisual.thickness);
        transform.update(mShader);
        drawable.draw();
    }

}