#pragma once

#include <shader.h>
#include <draw.h>
#include <image.h>
#include <frame.h>

namespace gl {

    struct Environment final {
        glm::ivec2 resolution;
        glm::ivec2 prefilter_resolution;
        glm::ivec2 irradiance_resolution = { 32, 32 };
        int prefilter_levels = 5;
        ImageBuffer hdr = GL_TEXTURE_2D;
        ImageBuffer skybox = GL_TEXTURE_CUBE_MAP;
        ImageBuffer irradiance = GL_TEXTURE_CUBE_MAP;
        ImageBuffer prefilter = GL_TEXTURE_CUBE_MAP;
        ImageBuffer brdf_convolution = GL_TEXTURE_2D;
        ImageParams params = {
                GL_CLAMP_TO_EDGE,
                GL_CLAMP_TO_EDGE,
                GL_CLAMP_TO_EDGE,
                { 0, 0, 0, 1 },
                GL_LINEAR,
                GL_LINEAR,
                -0.4f
        };

        void init();
        void free();
    };

    struct EnvRenderer final {
        Environment* env;

        void init(int w, int h);
        void free();

        void generate_env();

        void render();

    private:
        FrameBuffer fbo;
        Shader hdr_to_cubemap_shader;
        Shader hdr_irradiance_shader;
        Shader hdr_prefilter_convolution_shader;
        Shader brdf_convolution_shader;
        Shader env_shader;
        DrawableElements env_cube;
        DrawableQuad quad_drawable;
    };

}