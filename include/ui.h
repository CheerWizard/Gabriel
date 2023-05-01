#pragma once

#include <text.h>
#include <frame.h>
#include <entity.h>

namespace gl {

    struct UI_Pipeline final {
        ecs::Scene* scene;

        UI_Pipeline(ecs::Scene* scene) : scene(scene) {}

        ~UI_Pipeline() {
            free();
        }

        inline const ImageBuffer& get_render_target() const {
            return render_target;
        }

        void init(int w, int h);
        void free();

        void resize(int w, int h);

        void render();

    private:
        void init_frame(int w, int h);

    private:
        ImageBuffer render_target;
        FrameBuffer fbo;
        Text2dRenderer text2d_renderer;
        Text3dRenderer text3d_renderer;
    };

}