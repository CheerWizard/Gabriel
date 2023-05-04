#pragma once

#include <frame.h>
#include <draw.h>
#include <shader.h>

namespace gl {

    struct BloomUpsampleShader : Shader {
        UniformF filter_radius = { "filter_radius", 0.005f };

        void init();

        void update_filter_radius();
    };

    struct BloomDownsampleShader : Shader {
        UniformV2I resolution = { "resolution", { 800, 600 } };
        UniformI mip_level = { "mip_level", 1 };

        void init(const glm::ivec2& resolution);

        void update_resolution();
    };

    struct BloomMixShader : Shader {
        UniformF bloom_strength = { "bloom_strength", 0.02f };
        const ImageSampler hdr_sampler = { "hdr", 0 };
        ImageBuffer hdr_buffer;
        const ImageSampler bloom_sampler = { "bloom", 1 };
        ImageBuffer bloom_buffer;

        void init();
        void update();
        void update_bloom_strength();
    };

    struct BloomRenderer final {
        int mip_levels = 6;

        BloomRenderer(int width, int height);
        ~BloomRenderer();

        inline const ImageBuffer& get_render_target() {
            return render_target;
        }

        inline ImageBuffer& get_hdr_buffer() {
            return mix_shader.hdr_buffer;
        }

        inline ImageBuffer& get_bloom_buffer() {
            return mix_shader.bloom_buffer;
        }

        inline float& get_filter_radius() {
            return upsample_shader.filter_radius.value;
        }

        inline float& get_bloom_strength() {
            return mix_shader.bloom_strength.value;
        }

        void resize(int w, int h);

        void render();

    private:
        void init_mix_color();
        void init_mips();

        void render_downsample();
        void render_upsample();
        void render_mix();

    private:
        glm::ivec2 resolution;
        ImageBuffer render_target;
        FrameBuffer fbo;
        BloomDownsampleShader downsample_shader;
        BloomUpsampleShader upsample_shader;
        BloomMixShader mix_shader;
        DrawableQuad drawable;
        std::vector<ColorAttachment> mips;
        ColorAttachment mix_color;
    };

}