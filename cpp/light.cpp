#include <light.h>
#include <draw.h>

namespace gl {

    static Shader light_present_shader;
    static DrawableElements light_present_drawable;

    static FrameBuffer light_env_fbo;

    static Shader hdr_to_cubemap_shader;
    static Shader hdr_irradiance_shader;
    static Shader hdr_prefilter_convolution_shader;
    static Shader brdf_convolution_shader;

    static Shader env_shader;
    static DrawableElements env_cube;

    void LightPresent::init() {
        light_present_shader.init(
            "shaders/light_present.vert",
            "shaders/light_present.frag"
        );

        presentation.init_solid(light_present_drawable, color);
    }

    void LightPresent::free() {
        light_present_shader.free();
        light_present_drawable.free();
    }

    void LightPresent::update() {
        light_present_shader.use();
        transform.update(light_present_shader);
        light_present_drawable.draw();
    }

    void EnvLight::init() {
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
            "shaders/brdf_convolution.vert",
            "shaders/brdf_convolution.frag"
        );
        env_shader.init(
            "shaders/cubemap.vert",
            "shaders/hdr_cubemap.frag"
        );

        light_env_fbo.rbo = { 512, 512 };
        light_env_fbo.rbo.format = GL_DEPTH_COMPONENT24;
        light_env_fbo.rbo.type = GL_DEPTH_ATTACHMENT;
        light_env_fbo.init_with_render_buffer();
    }

    void EnvLight::free() {
        light_env_fbo.free();
        hdr_to_cubemap_shader.free();
        hdr_irradiance_shader.free();
        hdr_prefilter_convolution_shader.free();
        brdf_convolution_shader.free();
        env_shader.free();
        env_cube.free();

        env.free();
        irradiance.free();
        prefilter.free();
        brdf_convolution.free();
    }

    void EnvLight::generate(const Texture& hdr_texture) {
        int env_resolution = resolution;
        int env_prefilter_resolution = prefilter_resolution;
        // generate env maps
        gl::TextureParams env_map_params;
        env_map_params.s = GL_CLAMP_TO_EDGE;
        env_map_params.t = GL_CLAMP_TO_EDGE;
        env_map_params.r = GL_CLAMP_TO_EDGE;
        env_map_params.min_filter = GL_LINEAR;
        env_map_params.mag_filter = GL_LINEAR;
        irradiance.init_cubemap(
                { 32, 32, GL_RGB16F, GL_RGB, GL_FLOAT },
                env_map_params
        );
        brdf_convolution.init(
                { env_resolution, env_resolution, GL_RG16F, GL_RG, GL_FLOAT },
                env_map_params
        );
        env.init_cubemap(
                { env_resolution, env_resolution, GL_RGB16F, GL_RGB, GL_FLOAT },
                env_map_params
        );
        env_map_params.min_filter = GL_LINEAR_MIPMAP_LINEAR;
        prefilter.init_cubemap(
                { env_prefilter_resolution, env_prefilter_resolution, GL_RGB16F, GL_RGB, GL_FLOAT },
                env_map_params
        );
        // create unit cube for hdr conversions and env skybox
        CubeDefault().init(env_cube);
        glm::mat4 cube_perspective = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 cube_views[] = {
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };
        // create HDR env cube map
        light_env_fbo.bind();
        glViewport(0, 0, env_resolution, env_resolution);
        light_env_fbo.rbo.width = env_resolution;
        light_env_fbo.rbo.height = env_resolution;
        light_env_fbo.rbo.update();
        hdr_to_cubemap_shader.use();
        hdr_texture.bind();
        hdr_to_cubemap_shader.set_uniform_args("perspective", cube_perspective);
        for (int i = 0; i < 6; i++) {
            hdr_to_cubemap_shader.set_uniform_args("view", cube_views[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, env.id, 0);
            gl::clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            env_cube.draw();
        }
        env_map_params.min_filter = GL_LINEAR_MIPMAP_LINEAR;
        env.generate_mipmaps(env_map_params);
        // create HDR env irradiance map
        light_env_fbo.bind();
        glViewport(0, 0, 32, 32);
        light_env_fbo.rbo.width = 32;
        light_env_fbo.rbo.height = 32;
        light_env_fbo.rbo.update();
        hdr_irradiance_shader.use();
        env.update(hdr_irradiance_shader);
        hdr_irradiance_shader.set_uniform_args("perspective", cube_perspective);
        for (int i = 0; i < 6; i++) {
            hdr_irradiance_shader.set_uniform_args("view", cube_views[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradiance.id, 0);
            gl::clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            env_cube.draw();
        }
        // create HDR env prefilter map
        light_env_fbo.bind();
        hdr_prefilter_convolution_shader.use();
        env.update(hdr_prefilter_convolution_shader);
        hdr_prefilter_convolution_shader.set_uniform_args("perspective", cube_perspective);
        int mip_w = env_prefilter_resolution * 2;
        int mip_h = env_prefilter_resolution * 2;
        for (int mip = 0 ; mip < prefilter_levels ; mip++) {
            // resize framebuffer according to mip size.
            mip_w /= 2;
            mip_h /= 2;
            glViewport(0, 0, mip_w, mip_h);
            light_env_fbo.rbo.width = mip_w;
            light_env_fbo.rbo.height = mip_h;
            light_env_fbo.rbo.update();
            // update roughness on each mip
            float roughness = (float) mip / (float) (prefilter_levels - 1);
            hdr_prefilter_convolution_shader.set_uniform_args("roughness", roughness);
            float resolution_float = (float) env_resolution;
            hdr_prefilter_convolution_shader.set_uniform_args("resolution", resolution_float);
            // capture mip texture on each cube face
            for (int i = 0; i < 6; i++) {
                hdr_prefilter_convolution_shader.set_uniform_args("view", cube_views[i]);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilter.id, mip);
                gl::clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                env_cube.draw();
            }
        }
        // create env BRDF convolution map
        VertexArray brdf_conv_rect_vao;
        brdf_conv_rect_vao.init();
        light_env_fbo.bind();
        glViewport(0, 0, env_resolution, env_resolution);
        light_env_fbo.rbo.width = env_resolution;
        light_env_fbo.rbo.height = env_resolution;
        light_env_fbo.rbo.update();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdf_convolution.id, 0);
        brdf_convolution_shader.use();
        gl::clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        brdf_conv_rect_vao.draw_quad();

        FrameBuffer::unbind();
    }

    void EnvLight::update(Shader& shader) {
        shader.use();

        shader.set_uniform_struct_args("envlight", "prefilter_levels", prefilter_levels);

        shader.set_uniform_struct_args("envlight", irradiance.sampler.name, irradiance.sampler.value);
        irradiance.bind();

        shader.set_uniform_struct_args("envlight", prefilter.sampler.name, prefilter.sampler.value);
        prefilter.bind();

        shader.set_uniform_struct_args("envlight", brdf_convolution.sampler.name, brdf_convolution.sampler.value);
        brdf_convolution.bind();
    }

    void EnvLight::render() {
        glDepthFunc(GL_LEQUAL);
        env_shader.use();
        glBindTexture(env.type, env.id);
        env_cube.draw();
        glDepthFunc(GL_LESS);
    }

}