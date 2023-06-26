#include <debugging/light_visual.h>

namespace gl {

    LightVisualRenderer::LightVisualRenderer() {
        mShader.addVertexStage("shaders/light_present.vert");
        mShader.addFragmentStage("shaders/light_present.frag");
        mShader.complete();
    }

    LightVisualRenderer::~LightVisualRenderer() {
        mShader.free();
    }

    void LightVisualRenderer::begin() {
        mShader.use();
    }

    void LightVisualRenderer::render(LightVisual &lightVisual) {
        mShader.setUniformArgs("entity_id", lightVisual.entityId);
        mShader.setUniformArgs("color", lightVisual.color);
        lightVisual.transform.update(mShader);
        lightVisual.drawable.draw();
    }

}