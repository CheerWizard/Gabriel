#pragma once

#include <shader.h>
#include <buffers.h>
#include <frame.h>
#include <transform.h>
#include <draw.h>
#include <model_loader.h>
#include <scene.h>

namespace gl {

    struct DirectShadow : ecs::Component {
        glm::vec3 direction;
        glm::mat4 light_space;

        void update();
    };

    struct DirectShadowRenderer final {
        ImageBuffer render_target;

        void init(int w, int h);
        void free();

        void resize(int w, int h);

        void begin();
        void end();

        void update(DirectShadow& direct_shadow);
        void render(Transform& transform, DrawableElements& drawable);

    private:
        FrameBuffer fbo;
        Shader shader;
    };

    struct PointShadow : ecs::Component {
        int width = 1024;
        int height = 1024;
        glm::vec3 position;
        float far_plane = 25;
        std::vector<glm::mat4> light_spaces;

        void update();
    };

    struct PointShadowRenderer final {
        ImageBuffer render_target;

        void init(int w, int h);
        void free();

        void resize(int w, int h);

        void begin();
        void end();

        void update(PointShadow& point_shadow);
        void render(Transform& transform, DrawableElements& drawable);

    private:
        FrameBuffer fbo;
        Shader shader;
    };

}