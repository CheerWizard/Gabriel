#pragma once

#include <frame.h>
#include <buffers.h>
#include <draw.h>
#include <shader.h>

namespace gl {

    struct Bloom final {
        Texture render_target;
        Texture src;

        glm::ivec2 resolution;
        int mip_levels = 6;
        float filter_radius = 0.005f;
        float bloom_strength = 0.04f;

        void init();
        void free();

        void render();

        void resize(int w, int h);

    private:
        void init_mix_color();
        void init_mips();

        void set_resolution(const glm::ivec2& resolution);
        void set_filter_radius(float filter_radius);
        void set_bloom_strength(float bloom_strength);

        void render_downsample();
        void render_upsample();
        void render_mix();

    private:
        FrameBuffer fbo;
        VertexArray vao;
        Shader downsample_shader;
        Shader upsample_shader;
        Shader mix_shader;
        std::vector<ColorAttachment> mips;
        ColorAttachment mix_color;
    };

}