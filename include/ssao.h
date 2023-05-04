#pragma once

#include <draw.h>
#include <shader.h>
#include <frame.h>

namespace gl {

    struct SsaoParams final {
        const ImageSampler positions_sampler = { "v_positions", 0 };
        ImageBuffer positions;

        const ImageSampler normals_sampler = { "v_normals", 1 };
        ImageBuffer normals;

        const ImageSampler noise_sampler = { "noise", 2 };
        ImageBuffer noise;

        UniformI noise_size = { "noise_size", 4 };

        UniformV2F resolution = { "resolution", { 512, 512 } };

        UniformArrayV3F samples = { "samples", 64 };
        UniformI samples_size = { "samples_size", 64 };
        UniformF sample_radius  = { "sample_radius", 1 };
        UniformF sample_bias  = { "sample_bias", 0.025f };

        UniformI occlusion_power  = { "occlusion_power", 4 };

        void init();
        void free();
    };

    struct SsaoShader : Shader {
        SsaoParams params;

        void init(int width, int height);
        void update();

        void resize(int width, int height);
    };

    struct SsaoBlurShader : Shader {
        void init();
        void update(const ImageBuffer& ssao_image);
    };

    struct SsaoRenderer final {

        SsaoRenderer(int width, int height);
        ~SsaoRenderer();

        inline SsaoParams& get_params() {
            return ssao_shader.params;
        }

        inline const ImageBuffer& get_render_target() {
            return render_target;
        }

        void resize(int w, int h);

        void render();

    private:
        ImageBuffer render_target;
        ImageBuffer ssao_image;
        SsaoShader ssao_shader;
        SsaoBlurShader blur_shader;
        FrameBuffer fbo;
        FrameBuffer blur_fbo;
        DrawableQuad drawable;
    };

}