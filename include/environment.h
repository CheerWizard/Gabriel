#pragma once

#include <shader.h>
#include <draw.h>
#include <texture.h>
#include <frame.h>

namespace gl {

    struct Environment final {
        glm::ivec2 resolution;
        glm::ivec2 prefilter_resolution;
        glm::ivec2 irradiance_resolution = { 32, 32 };
        int prefilter_levels = 5;
        Texture hdr = { 0, GL_TEXTURE_CUBE_MAP };
        Texture skybox = {0, GL_TEXTURE_CUBE_MAP };
        Texture irradiance = { 0, GL_TEXTURE_CUBE_MAP };
        Texture prefilter = { 0, GL_TEXTURE_CUBE_MAP };
        Texture brdf_convolution = { 0, GL_TEXTURE_2D };
        TextureParams params = {
                GL_CLAMP_TO_EDGE,
                GL_CLAMP_TO_EDGE,
                GL_CLAMP_TO_EDGE,
                { 1, 1, 1, 1 },
                GL_LINEAR,
                GL_LINEAR,
                false,
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
        VertexArray brdf_vao;
    };

}