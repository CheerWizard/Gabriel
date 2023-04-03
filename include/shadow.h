#pragma once

#include <shader.h>
#include <buffers.h>
#include <frame.h>
#include <transform.h>
#include <draw.h>
#include <model_loader.h>

namespace gl {

    struct DirectShadow final {
        Transform transform;
        DrawableElements drawable;
        glm::vec3 direction;
        glm::mat4 light_space;

        void update();
    };

    struct DirectShadowRenderer final {
        Texture render_target;

        void init(int w, int h);
        void free();

        void resize(int w, int h);

        void begin();
        void end();

        void update(DirectShadow& direct_shadow);
        void render(DirectShadow& direct_shadow);

    private:
        FrameBuffer fbo;
        Shader shader;
    };

    struct PointShadow final {
        Transform transform;
        DrawableElements drawable;
        int width = 1024;
        int height = 1024;
        glm::vec3 position;
        float far_plane = 25;
        std::vector<glm::mat4> light_spaces;

        void update();
    };

    struct PointShadowRenderer final {
        Texture render_target;

        void init(int w, int h);
        void free();

        void resize(int w, int h);

        void begin();
        void end();

        void update(PointShadow& point_shadow);
        void render(PointShadow& point_shadow);

    private:
        Shader shader;
        FrameBuffer fbo;
    };

}