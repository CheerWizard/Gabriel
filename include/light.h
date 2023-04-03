#pragma once

#include <geometry.h>
#include <transform.h>
#include <camera.h>
#include <frame.h>
#include <draw.h>

namespace gl {

    struct LightPresent final {
        u32 id = 0;
        glm::vec4 color = { 1, 1, 1, 1 };
        Transform transform = {
                { 0, 0, -2 },
                { 0, 0, 0 },
                { 0.1, 0.1, 0.1 },
        };
        CubeDefault presentation;

        void init();
        void free();

        void update();
    };

    struct PhongLight final {
        glm::vec4 position = { 0, 0, 0, 1 };
        glm::vec4 color = { 1, 1, 1, 1 };
    };

    struct DirectLight final {
        glm::vec4 direction = { -2.0f, 4.0f, -1.0f, 0 };
        glm::vec4 color = { 1, 1, 1, 1 };
    };

    struct PointLight final {
        glm::vec4 position = { 0, 0, 0, 0 };
        glm::vec4 color = { 1, 1, 1, 1 };
        float constant = 0;
        float linear = 0;
        float quadratic = 1;
        float refraction = 1;
    };

    struct SpotLight final {
        glm::vec4 position = { 0, 0, 0, 0 };
        glm::vec4 direction = { -0.2f, -1.0f, -0.3f, 0 };
        glm::vec4 color = { 1, 1, 1, 1 };
        float cutoff = glm::cos(glm::radians(6.5f));
        float outer = glm::cos(glm::radians(11.5f));
        float refraction = 1;
        float pad = 0;
    };

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