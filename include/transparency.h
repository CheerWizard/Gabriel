#pragma once

#include <frame.h>
#include <shader.h>
#include <draw.h>
#include <transform.h>
#include <material.h>

namespace gl {

    struct TransparentBuffer final {
        Texture accumulation;
        Texture revealage;
    };

    struct TransparentRenderer final {
        Texture render_target;
        TransparentBuffer transparent_buffer;

        void init(Shader& shader, int w, int h);
        void free();

        void begin();
        void end();

        void resize(int w, int h);

        void render(u32 object_id, DrawableElements* drawable, Transform* transform);
        void render(u32 object_id, DrawableElements* drawable, Transform* transform, Material* material);

    private:
        VertexArray vao;
        Shader shader;
        FrameBuffer fbo;
        Shader composite_shader;
        FrameBuffer composite_fbo;
    };

}