#pragma once

#include <normal_visual.h>
#include <polygon_visual.h>
#include <frame.h>
#include <entity.h>

namespace gl {

    struct DebugControlPipeline final {
        ecs::Scene* scene;

        DebugControlPipeline(ecs::Scene* scene) : scene(scene) {}

        ~DebugControlPipeline() {
            free();
        }

        inline const ImageBuffer& get_render_target() {
            return render_target;
        }

        void init(int w, int h);
        void free();

        void resize(int w, int h);

        void render();

    private:
        void init_frame(int w, int h);

    private:
        FrameBuffer fbo;
        ImageBuffer render_target;
        PolygonVisualRenderer polygon_visual_renderer;
        NormalVisualRenderer normal_visual_renderer;
    };

}