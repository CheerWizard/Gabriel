#include <environment.h>
#include <cube.h>

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
        params.min_filter = GL_LINEAR;
        brdf_convolution.init(
                { resolution.x, resolution.y, GL_RG16F, GL_RG, GL_FLOAT },
                params
        );
        skybox.init_cubemap(
                { resolution.x, resolution.y, GL_RGB16F, GL_RGB, GL_FLOAT },
                params
        );
        irradiance.init_cubemap(
                { irradiance_resolution.x, irradiance_resolution.y, GL_RGB16F, GL_RGB, GL_FLOAT },
                params
        );

        params.min_filter = GL_LINEAR_MIPMAP_LINEAR;
        prefilter.init_cubemap(
                { prefilter_resolution.x, prefilter_resolution.y, GL_RGB16F, GL_RGB, GL_FLOAT },
                params
        );
    }

    void Environment::free() {
        hdr.free();
        skybox.free();
        irradiance.free();
        prefilter.free();
        brdf_convolution.free();
    }

    void EnvRenderer::init(int w, int h) {
        hdr_to_cubemap_shader.init(
                "shaders/hdr_to_cubemap.vert",
                "shaders/hdr_to_cubemap.frag"
        );
        hdr_irradiance_shader.init(
                "shaders/hdr_irradiance.vert",
                "shaders/hdr_irradiance.frag"
        );
        hdr_prefilter_convolution_shader.init(
                "shaders/hdr_prefilter_convolution.vert",
                "shaders/hdr_prefilter_convolution.frag"
        );
        brdf_convolution_shader.init(
                "shaders/fullscreen_quad.vert",
                "shaders/brdf_convolution.frag"
        );
        env_shader.init(
                "shaders/cubemap.vert",
                "shaders/hdr_cubemap.frag"
        );

        fbo.rbo = { w, h };
        fbo.rbo.format = GL_DEPTH_COMPONENT24;
        fbo.rbo.type = GL_DEPTH_ATTACHMENT;
        fbo.init_with_render_buffer();

        CubeDefault().init(env_cube);

        brdf_vao.init();
    }

    void EnvRenderer::free() {
        hdr_to_cubemap_shader.free();
        hdr_irradiance_shader.free();
        hdr_prefilter_convolution_shader.free();
        brdf_convolution_shader.free();
        env_shader.free();
        env_cube.free();
        brdf_vao.free();
    }

    void EnvRenderer::generate_env() {
        fbo.bind();
        // create HDR skybox cube map
        glViewport(0, 0, env->resolution.x, env->resolution.y);
        fbo.rbo.width = env->resolution.x;
        fbo.rbo.height = env->resolution.y;
        fbo.rbo.update();
        hdr_to_cubemap_shader.use();
        hdr_to_cubemap_shader.bind_sampler("sampler", 0, env->hdr);
        hdr_to_cubemap_shader.set_uniform_args("perspective", cube_perspective);
        for (int i = 0; i < 6; i++) {
            hdr_to_cubemap_shader.set_uniform_args("view", cube_views[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, env->skybox.id, 0);
            clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            env_cube.draw();
        }
        env->skybox.generate_mipmaps(env->params);
        // create HDR skybox irradiance map
        glViewport(0, 0, env->irradiance_resolution.x, env->irradiance_resolution.y);
        fbo.rbo.width = env->irradiance_resolution.x;
        fbo.rbo.height = env->irradiance_resolution.y;
        fbo.rbo.update();
        hdr_irradiance_shader.use();
        hdr_irradiance_shader.bind_sampler("sampler", 0, env->skybox);
        hdr_irradiance_shader.set_uniform_args("perspective", cube_perspective);
        for (int i = 0; i < 6; i++) {
            hdr_irradiance_shader.set_uniform_args("view", cube_views[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, env->irradiance.id, 0);
            clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            env_cube.draw();
        }
        // create HDR skybox prefilter map
        hdr_prefilter_convolution_shader.use();
        hdr_prefilter_convolution_shader.bind_sampler("sampler", 0, env->skybox);
        hdr_prefilter_convolution_shader.set_uniform_args("perspective", cube_perspective);
        int mip_w = env->prefilter_resolution.x * 2;
        int mip_h = env->prefilter_resolution.y * 2;
        for (int mip = 0 ; mip < env->prefilter_levels ; mip++) {
            // resize framebuffer according to mip size.
            mip_w /= 2;
            mip_h /= 2;
            glViewport(0, 0, mip_w, mip_h);
            fbo.rbo.width = mip_w;
            fbo.rbo.height = mip_h;
            fbo.rbo.update();
            // update roughness on each mip
            float roughness = (float) mip / (float) (env->prefilter_levels - 1);
            hdr_prefilter_convolution_shader.set_uniform_args("roughness", roughness);
            float resolution_float = (float) env->resolution.x;
            hdr_prefilter_convolution_shader.set_uniform_args("resolution", resolution_float);
            // capture mip texture on each cube face
            for (int i = 0; i < 6; i++) {
                hdr_prefilter_convolution_shader.set_uniform_args("view", cube_views[i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, env->prefilter.id, mip);
                clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                env_cube.draw();
            }
        }

        // create skybox BRDF convolution map
        glViewport(0, 0, env->resolution.x, env->resolution.y);

        fbo.rbo.width = env->resolution.x;
        fbo.rbo.height = env->resolution.y;
        fbo.rbo.update();

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, env->brdf_convolution.id, 0);

        clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        brdf_convolution_shader.use();
        brdf_vao.draw_quad();

        FrameBuffer::unbind();
    }

    void EnvRenderer::render() {
        glDepthFunc(GL_LEQUAL);
        env_shader.use();
        env->skybox.activate(0);
        env->skybox.bind();
        env_cube.draw();
        glDepthFunc(GL_LESS);
    }

}