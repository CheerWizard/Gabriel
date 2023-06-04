#include <debugging/polygon_visual.h>

namespace gl {

    PolygonFace PolygonVisualRenderer::polygonFace = PolygonFace::FRONT;
    PolygonType PolygonVisualRenderer::polygonType = PolygonType::LINE;

    PolygonVisualRenderer::PolygonVisualRenderer() {
        mShader.addVertexStage("shaders/polygon_visual/polygon_visual.vert");
        mShader.addFragmentStage("shaders/polygon_visual/polygon_visual.frag");
        mShader.complete();

        mText2dShader.addVertexStage("shaders/polygon_visual/text2d_visual.vert");
        mText2dShader.addFragmentStage("shaders/polygon_visual/text2d_visual.frag");
        mText2dShader.complete();

        mText3dShader.addVertexStage("shaders/polygon_visual/text3d_visual.vert");
        mText3dShader.addFragmentStage("shaders/polygon_visual/text3d_visual.frag");
        mText3dShader.complete();

        PolygonMode::enable();
    }

    PolygonVisualRenderer::~PolygonVisualRenderer() {
        mShader.free();
        mText2dShader.free();
        mText3dShader.free();
    }

    void PolygonVisualRenderer::begin() {
        PolygonMode::set(PolygonFace::FRONT, PolygonType::LINE);
        mShader.use();
    }

    void PolygonVisualRenderer::end() {
        PolygonMode::set(PolygonFace::FRONT, PolygonType::FILL);
    }

    void PolygonVisualRenderer::render(Drawable2dVisual& drawableVisual) {
        mShader.setUniformArgs("color", drawableVisual.color);
        mShader.setUniformArgs("thickness", drawableVisual.thickness);
        drawableVisual.transform->update(mShader);
        drawableVisual.drawable->draw();
    }

    void PolygonVisualRenderer::render(Drawable3dVisual& drawableVisual) {
        mShader.setUniformArgs("color", drawableVisual.color);
        mShader.setUniformArgs("thickness", drawableVisual.thickness);
        drawableVisual.transform->update(mShader);
        drawableVisual.drawable->draw();
    }

    void PolygonVisualRenderer::render(Text2dVisual& textVisual) {
        mText2dShader.setUniformArgs("color", textVisual.color);
        mText2dShader.setUniformArgs("thickness", textVisual.thickness);
        textVisual.transform->update(mText2dShader);
        textVisual.drawable->draw();
    }

    void PolygonVisualRenderer::render(Text3dVisual& textVisual) {
        mText3dShader.setUniformArgs("color", textVisual.color);
        mText3dShader.setUniformArgs("thickness", textVisual.thickness);
        textVisual.transform->update(mText3dShader);
        textVisual.drawable->draw();
    }

}