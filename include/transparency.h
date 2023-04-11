#pragma once

#include <frame.h>
#include <shader.h>
#include <draw.h>
#include <transform.h>
#include <material.h>

namespace gl {

    struct TransparentBuffer final {
        ImageBuffer accumulation;
        ImageBuffer revealage;
    };

    struct TransparentRenderer final {
        ImageBuffer render_target;
        TransparentBuffer transparent_buffer;

        void init(Shader& shader, int w, int h);
        void free();

        void begin();
        void end();

        void resize(int w, int h);

        void render(ecs::EntityID entity_id, Transform& transform, DrawableElements& drawable);
        void render(ecs::EntityID entity_id, Transform& transform, DrawableElements& drawable, Material& material);

    private:
        VertexArray vao;
        Shader shader;
        FrameBuffer fbo;
        Shader composite_shader;
        FrameBuffer composite_fbo;
    };

}