#include <pbr/environment.h>

#include <geometry/cube.h>

namespace gl {

    // create unit cube projection and view
    static glm::mat4 cube_perspective = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    static glm::mat4 cube_views[] = {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    void Environment::init() {
        params.minFilter = GL_LINEAR;

        brdfConvolution.init();
        Image brdf_image = { resolution.x, resolution.y, 2, GL_RG16F, GL_RG, PixelType::FLOAT };
        brdfConvolution.load(brdf_image, params);

        skybox.init();
        Image skybox_image = { resolution.x, resolution.y, 3, GL_RGB16F, GL_RGB, PixelType::FLOAT };
        skybox.loadCubemap(skybox_image, params);

        irradiance.init();
        Image irradiance_image = { irradianceResolution.x, irradianceResolution.y, 3, GL_RGB16F, GL_RGB, PixelType::FLOAT };
        irradiance.loadCubemap(irradiance_image, params);

        params.minFilter = GL_LINEAR_MIPMAP_LINEAR;

        prefilter.init();
        Image prefilter_image = { prefilterResolution.x, prefilterResolution.y, 3, GL_RGB16F, GL_RGB, PixelType::FLOAT  };
        prefilter.loadCubemap(prefilter_image, params);
    }

    void Environment::free() {
        hdr.free();
        skybox.free();
        irradiance.free();
        prefilter.free();
        brdfConvolution.free();
    }

    EnvRenderer::EnvRenderer(int width, int height) {
        mHdrToCubemapShader.addVertexStage("shaders/hdr_to_cubemap.vert");
        mHdrToCubemapShader.addFragmentStage("shaders/hdr_to_cubemap.frag");
        mHdrToCubemapShader.complete();

        mHdrIrradianceShader.addVertexStage("shaders/hdr_irradiance.vert");
        mHdrIrradianceShader.addFragmentStage("shaders/hdr_irradiance.frag");
        mHdrIrradianceShader.complete();

        mHdrPrefilterConvolutionShader.addVertexStage("shaders/hdr_prefilter_convolution.vert");
        mHdrPrefilterConvolutionShader.addFragmentStage("shaders/hdr_prefilter_convolution.frag");
        mHdrPrefilterConvolutionShader.complete();

        mBrdfConvolutionShader.addVertexStage("shaders/fullscreen_quad.vert");
        mBrdfConvolutionShader.addFragmentStage("shaders/brdf_convolution.frag");
        mBrdfConvolutionShader.complete();

        mEnvShader.addVertexStage("shaders/cubemap.vert");
        mEnvShader.addFragmentStage("shaders/hdr_cubemap.frag");
        mEnvShader.complete();

        mFrame.rbo = { width, height };
        mFrame.rbo.format = GL_DEPTH_COMPONENT24;
        mFrame.rbo.type = GL_DEPTH_ATTACHMENT;

        mFrame.initWithRenderBuffer();

        CubeDefault().init(mEnvCube);

        mDrawable.init();
    }

    EnvRenderer::~EnvRenderer() {
        mHdrToCubemapShader.free();
        mHdrIrradianceShader.free();
        mHdrPrefilterConvolutionShader.free();
        mBrdfConvolutionShader.free();
        mEnvShader.free();
        mEnvCube.free();
        mDrawable.free();
    }

    void EnvRenderer::generate() {
        mFrame.bind();
        // create HDR skybox cube map
        glViewport(0, 0, environment->resolution.x, environment->resolution.y);
        mFrame.rbo.width = environment->resolution.x;
        mFrame.rbo.height = environment->resolution.y;
        mFrame.rbo.update();
        mHdrToCubemapShader.use();
        mHdrToCubemapShader.bindSampler("sampler", 0, environment->hdr);
        mHdrToCubemapShader.setUniformArgs("perspective", cube_perspective);
        for (int i = 0; i < 6; i++) {
            mHdrToCubemapShader.setUniformArgs("view", cube_views[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, environment->skybox.id, 0);
            clearDisplay(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            mEnvCube.draw();
        }
        environment->skybox.generateMipmaps(environment->params);
        // create HDR skybox irradiance map
        glViewport(0, 0, environment->irradianceResolution.x, environment->irradianceResolution.y);
        mFrame.rbo.width = environment->irradianceResolution.x;
        mFrame.rbo.height = environment->irradianceResolution.y;
        mFrame.rbo.update();
        mHdrIrradianceShader.use();
        mHdrIrradianceShader.bindSampler("sampler", 0, environment->skybox);
        mHdrIrradianceShader.setUniformArgs("perspective", cube_perspective);
        for (int i = 0; i < 6; i++) {
            mHdrIrradianceShader.setUniformArgs("view", cube_views[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, environment->irradiance.id, 0);
            clearDisplay(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            mEnvCube.draw();
        }
        // create HDR skybox prefilter map
        mHdrPrefilterConvolutionShader.use();
        mHdrPrefilterConvolutionShader.bindSampler("sampler", 0, environment->skybox);
        mHdrPrefilterConvolutionShader.setUniformArgs("perspective", cube_perspective);
        int mip_w = environment->prefilterResolution.x * 2;
        int mip_h = environment->prefilterResolution.y * 2;
        for (int mip = 0 ; mip < environment->prefilterLevels ; mip++) {
            // resize framebuffer according to mip size.
            mip_w /= 2;
            mip_h /= 2;
            glViewport(0, 0, mip_w, mip_h);
            mFrame.rbo.width = mip_w;
            mFrame.rbo.height = mip_h;
            mFrame.rbo.update();
            // update roughness on each mip
            float roughness = (float) mip / (float) (environment->prefilterLevels - 1);
            mHdrPrefilterConvolutionShader.setUniformArgs("roughness", roughness);
            float resolution_float = (float) environment->resolution.x;
            mHdrPrefilterConvolutionShader.setUniformArgs("resolution", resolution_float);
            // capture mip texture on each cube face
            for (int i = 0; i < 6; i++) {
                mHdrPrefilterConvolutionShader.setUniformArgs("view", cube_views[i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, environment->prefilter.id, mip);
                clearDisplay(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                mEnvCube.draw();
            }
        }

        // create skybox BRDF convolution map
        Viewport::resize(0, 0, environment->resolution.x, environment->resolution.y);

        mFrame.rbo.width = environment->resolution.x;
        mFrame.rbo.height = environment->resolution.y;
        mFrame.rbo.update();

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, environment->brdfConvolution.id, 0);

        clearDisplay(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mBrdfConvolutionShader.use();
        mDrawable.draw();

        FrameBuffer::unbind();
    }

    void EnvRenderer::render() {
        glDepthFunc(GL_LEQUAL);
        mEnvShader.use();
        environment->skybox.activate(0);
        environment->skybox.bind();
        mEnvCube.draw();
        glDepthFunc(GL_LESS);
    }

}