#pragma once

#include <api/shader.h>

namespace gl {

    struct GABRIEL_API RayTraceRenderer final {

        RayTraceRenderer(int w, int h);
        ~RayTraceRenderer();

        [[nodiscard]] inline const ImageBuffer& getRenderTarget() const {
            return m_renderTarget;
        }

        void resize(int w, int h);

        void render();

    private:
        ImageBuffer m_renderTarget;
        Image m_output;
        ComputeShader m_shader;
    };

}