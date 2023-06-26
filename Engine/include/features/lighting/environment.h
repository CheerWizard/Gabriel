#pragma once

#include <api/shader.h>
#include <api/draw.h>
#include <api/image.h>
#include <api/frame.h>

namespace gl {

    struct GABRIEL_API Environment final {
        glm::ivec2 resolution;
        glm::ivec2 prefilterResolution;
        glm::ivec2 irradianceResolution = {32, 32 };
        int prefilterLevels = 5;
        ImageBuffer hdr = GL_TEXTURE_2D;
        ImageBuffer skybox = GL_TEXTURE_CUBE_MAP;
        ImageBuffer irradiance = GL_TEXTURE_CUBE_MAP;
        ImageBuffer prefilter = GL_TEXTURE_CUBE_MAP;
        ImageBuffer brdfConvolution = GL_TEXTURE_2D;
        ImageParams params = {
                GL_CLAMP_TO_EDGE,
                GL_CLAMP_TO_EDGE,
                GL_CLAMP_TO_EDGE,
                { 0, 0, 0, 1 },
                GL_LINEAR,
                GL_LINEAR,
                -0.4f
        };
        bool enable = true;

        Environment();
        ~Environment();

        void init();

        void makeResident();
        void makeNonResident();

    private:
        void free();
    };

    struct GABRIEL_API EnvUniform final {
        int prefilterLevels;
        u64 irradiance;
        u64 prefilter;
        u64 brdf_convolution;
    };

    struct GABRIEL_API EnvStorage final {

        static Environment* environment;

        static void init();
        static void free();

        static void update();

    private:
        static UniformBuffer sEnvUbo;
    };

    struct GABRIEL_API EnvRenderer final {
        Environment* environment = null;

        EnvRenderer(int width, int height);
        ~EnvRenderer();

        void generate();

        void render();

        void setEnvironment(Environment* environment);

    private:
        FrameBuffer mFrame;
        DrawableQuad mDrawable;
        Shader mHdrToCubemapShader;
        Shader mHdrIrradianceShader;
        Shader mHdrPrefilterConvolutionShader;
        Shader mBrdfConvolutionShader;
        Shader mEnvShader;
        DrawableElements mEnvCube;
    };

}