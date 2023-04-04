#include <pbr.h>
#include <array>

namespace gl {

    static std::array<TextureSampler, 6> samplers = {
            TextureSampler { "positions", 2 },
            TextureSampler { "normals", 3 },
            TextureSampler { "albedos", 4 },
            TextureSampler { "pbr_params", 5 },
            TextureSampler { "shadow_proj_coords", 6 },
            TextureSampler { "ssao", 7 }
    };

    void PBR_Skeletal_ForwardRenderer::init() {
        shader.init(
                "shaders/skeletal.vert",
                "shaders/pbr.frag"
        );
    }

    void PBR_Skeletal_ForwardRenderer::update(Environment* env) {
        shader.use();
        shader.set_uniform_struct_args("envlight", "prefilter_levels", env->prefilter_levels);
        shader.bind_sampler_struct("envlight", "irradiance", 8, env->irradiance);
        shader.bind_sampler_struct("envlight", "prefilter", 9, env->prefilter);
        shader.bind_sampler_struct("envlight", "brdf_convolution", 10, env->brdf_convolution);
    }

    void PBR_Skeletal_DeferredRenderer::init() {
        shader.init(
                "shaders/skeletal_deferred.vert",
                "shaders/pbr_material.frag"
        );
    }

    void PBR_Skeletal_DeferredRenderer::update(Environment* env) {
        shader.use();
        shader.set_uniform_struct_args("envlight", "prefilter_levels", env->prefilter_levels);
        shader.bind_sampler_struct("envlight", "irradiance", 8, env->irradiance);
        shader.bind_sampler_struct("envlight", "prefilter", 9, env->prefilter);
        shader.bind_sampler_struct("envlight", "brdf_convolution", 10, env->brdf_convolution);
    }

    void PBR_ForwardRenderer::init(int w, int h) {
        shader.init(
                "shaders/pbr.vert",
                "shaders/pbr.frag"
        );
        // setup VAO
        vao.init();
        // setup PBR color
        ColorAttachment pbr_color = {0, w, h };
        pbr_color.data.internal_format = GL_RGBA16F;
        pbr_color.data.data_format = GL_RGBA;
        pbr_color.data.primitive_type = GL_FLOAT;
        pbr_color.params.min_filter = GL_LINEAR;
        pbr_color.params.mag_filter= GL_LINEAR;
        // setup PBR object ID
        ColorAttachment pbr_object_id = {2, w, h };
        pbr_object_id.data.internal_format = GL_R32UI;
        pbr_object_id.data.data_format = GL_RED_INTEGER;
        pbr_object_id.data.primitive_type = GL_UNSIGNED_INT;
        // setup PBR frame
        fbo.colors = { pbr_color, pbr_object_id };
        fbo.init_colors();
        fbo.rbo = { w, h };
        fbo.rbo.init();
        fbo.init();
        fbo.attach_colors();
        fbo.attach_render_buffer();
        fbo.complete();
        // setup PBR MSAA frame
        msaa_fbo = fbo;
        for (auto& color : msaa_fbo.colors) {
            color.data.samples = samples;
            color.view.type = GL_TEXTURE_2D_MULTISAMPLE;
            color.init();
        }
        msaa_fbo.rbo.samples = samples;
        msaa_fbo.init();
        msaa_fbo.rbo.init();
        msaa_fbo.attach_colors();
        msaa_fbo.rbo.attach();
        msaa_fbo.complete();

        render_target = fbo.colors[0].view;

        set_samples(samples);
    }

    void PBR_ForwardRenderer::free() {
        vao.free();
        shader.free();
        fbo.free();
        msaa_fbo.free();
    }

    void PBR_ForwardRenderer::resize(int w, int h) {
        fbo.resize(w, h);
        msaa_fbo.resize(w, h);
    }

    void PBR_ForwardRenderer::set_camera_pos(glm::vec3 &camera_pos) {
        shader.use();
        shader.set_uniform_args("camera_pos", camera_pos);
    }

    void PBR_ForwardRenderer::set_samples(int samples) {
        this->samples = samples;
        current_fbo = samples > 1 ? msaa_fbo : fbo;
    }

    PBR_Pixel PBR_ForwardRenderer::read_pixel(int x, int y) {
        PBR_Pixel pixel = fbo.read_pixel<PBR_Pixel>(2, x, y);
        return pixel;
    }

    void PBR_ForwardRenderer::bind() {
        current_fbo.bind();
//        clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

    void PBR_ForwardRenderer::unbind() {
//        compose_transparency();
        if (samples > 1) {
            int w = msaa_fbo.colors[0].data.width;
            int h = msaa_fbo.colors[0].data.height;
            FrameBuffer::blit(msaa_fbo.id, w, h, fbo.id, w, h, 1, GL_COLOR_BUFFER_BIT);
            FrameBuffer::blit(msaa_fbo.id, w, h, fbo.id, w, h, 1, GL_DEPTH_BUFFER_BIT);
        }
    }

    void PBR_ForwardRenderer::begin() {
        shader.use();
    }

    void PBR_ForwardRenderer::render(PBR_Component* pbr_component) {
        shader.bind_sampler("direct_shadow_sampler", 0, direct_shadow_map);
        shader.bind_sampler("point_shadow_sampler", 1, point_shadow_map);
        shader.set_uniform_args("far_plane", far_plane);
        shader.set_uniform_args("object_id", pbr_component->object_id);
        pbr_component->transform.update(shader);
        pbr_component->material->update(shader, 0);
        pbr_component->drawable->draw();
    }

    Shader& PBR_ForwardRenderer::get_shader() {
        return shader;
    }

    FrameBuffer& PBR_ForwardRenderer::get_current_frame() {
        return current_fbo;
    }

    void PBR_ForwardRenderer::render(PBR_Component* pbr_component, glm::mat4& light_space) {
        shader.set_uniform_args("direct_light_space", light_space);
        render(pbr_component);
    }

    void PBR_ForwardRenderer::update(Environment* env) {
        shader.use();
        shader.set_uniform_struct_args("envlight", "prefilter_levels", env->prefilter_levels);
        shader.bind_sampler_struct("envlight", "irradiance", 8, env->irradiance);
        shader.bind_sampler_struct("envlight", "prefilter", 9, env->prefilter);
        shader.bind_sampler_struct("envlight", "brdf_convolution", 10, env->brdf_convolution);
    }

    void PBR_DeferredRenderer::init(int w, int h) {
        vao.init();
        geometry_shader.init(
                "shaders/pbr_material.vert",
                "shaders/pbr_material.frag"
        );
        light_shader.init(
                "shaders/fullscreen_quad.vert",
                "shaders/pbr_light.frag"
        );
        // setup PBR positions
        ColorAttachment pbr_pos = {0, w, h };
        pbr_pos.data.internal_format = GL_RGBA32F;
        pbr_pos.data.data_format = GL_RGBA;
        pbr_pos.data.primitive_type = GL_FLOAT;
        pbr_pos.params.min_filter = GL_NEAREST;
        pbr_pos.params.mag_filter= GL_NEAREST;
        pbr_pos.params.s = GL_CLAMP_TO_EDGE;
        pbr_pos.params.t = GL_CLAMP_TO_EDGE;
        pbr_pos.params.r = GL_CLAMP_TO_EDGE;
        // setup PBR normals
        ColorAttachment pbr_normal = {1, w, h };
        pbr_normal.data.internal_format = GL_RGBA32F;
        pbr_normal.data.data_format = GL_RGBA;
        pbr_normal.data.primitive_type = GL_FLOAT;
        pbr_normal.params.min_filter = GL_NEAREST;
        pbr_normal.params.mag_filter= GL_NEAREST;
        // setup PBR albedos
        ColorAttachment pbr_albedo = {2, w, h };
        pbr_albedo.data.internal_format = GL_RGBA;
        pbr_albedo.data.data_format = GL_RGBA;
        pbr_albedo.data.primitive_type = GL_UNSIGNED_BYTE;
        pbr_albedo.params.min_filter = GL_NEAREST;
        pbr_albedo.params.mag_filter= GL_NEAREST;
        // setup PBR params
        ColorAttachment pbr_params = {3, w, h };
        pbr_params.data.internal_format = GL_RGBA;
        pbr_params.data.data_format = GL_RGBA;
        pbr_params.data.primitive_type = GL_UNSIGNED_BYTE;
        pbr_params.params.min_filter = GL_NEAREST;
        pbr_params.params.mag_filter= GL_NEAREST;
        // setup PBR shadow projection coords
        ColorAttachment pbr_shadow_proj = {4, w, h };
        pbr_shadow_proj.data.internal_format = GL_RGBA32F;
        pbr_shadow_proj.data.data_format = GL_RGBA;
        pbr_shadow_proj.data.primitive_type = GL_FLOAT;
        pbr_shadow_proj.params.min_filter = GL_NEAREST;
        pbr_shadow_proj.params.mag_filter= GL_NEAREST;
        // setup PBR object ID
        ColorAttachment pbr_object_id = { 5, w, h };
        pbr_object_id.data.internal_format = GL_R32UI;
        pbr_object_id.data.data_format = GL_RED_INTEGER;
        pbr_object_id.data.primitive_type = GL_UNSIGNED_INT;
        // setup PBR view positions
        ColorAttachment pbr_view_pos = {6, w, h };
        pbr_view_pos.data.internal_format = GL_RGBA32F;
        pbr_view_pos.data.data_format = GL_RGBA;
        pbr_view_pos.data.primitive_type = GL_FLOAT;
        pbr_view_pos.params.min_filter = GL_NEAREST;
        pbr_view_pos.params.mag_filter= GL_NEAREST;
        pbr_view_pos.params.s = GL_CLAMP_TO_EDGE;
        pbr_view_pos.params.t = GL_CLAMP_TO_EDGE;
        pbr_view_pos.params.r = GL_CLAMP_TO_EDGE;
        // setup PBR view normals
        ColorAttachment pbr_view_normal = {7, w, h };
        pbr_view_normal.data.internal_format = GL_RGBA32F;
        pbr_view_normal.data.data_format = GL_RGBA;
        pbr_view_normal.data.primitive_type = GL_FLOAT;
        pbr_view_normal.params.min_filter = GL_NEAREST;
        pbr_view_normal.params.mag_filter= GL_NEAREST;
        // setup PBR material frame
        geometry_fbo.colors = {
                pbr_pos,
                pbr_normal,
                pbr_albedo,
                pbr_params,
                pbr_shadow_proj,
                pbr_object_id,
                pbr_view_pos,
                pbr_view_normal
        };
        geometry_fbo.init_colors();
        geometry_fbo.rbo = { w, h };
        geometry_fbo.rbo.init();
        geometry_fbo.init();
        geometry_fbo.attach_colors();
        geometry_fbo.attach_render_buffer();
        geometry_fbo.complete();
        // setup PBR material MSAA frame
        geometry_msaa_fbo = geometry_fbo;
        for (auto& color : geometry_msaa_fbo.colors) {
            color.data.samples = samples;
            color.view.type = GL_TEXTURE_2D_MULTISAMPLE;
            color.init();
        }
        geometry_msaa_fbo.rbo.samples = samples;
        geometry_msaa_fbo.rbo.init();
        geometry_msaa_fbo.init();
        geometry_msaa_fbo.attach_colors();
        geometry_msaa_fbo.attach_render_buffer();
        geometry_msaa_fbo.complete();
        // setup PBR light color
        ColorAttachment pbr_light_color = { 0, w, h };
        pbr_light_color.data.internal_format = GL_RGBA16F;
        pbr_light_color.data.data_format = GL_RGBA;
        pbr_light_color.data.primitive_type = GL_FLOAT;
        pbr_light_color.params.min_filter = GL_LINEAR;
        pbr_light_color.params.mag_filter= GL_LINEAR;
        // setup PBR light frame
        light_fbo.colors = { pbr_light_color };
        light_fbo.init_colors();
        light_fbo.rbo = { w, h };
        light_fbo.rbo.init();
        light_fbo.init();
        light_fbo.attach_colors();
        light_fbo.attach_render_buffer();
        light_fbo.complete();
        // PBR light pass upload
        {
            light_shader.use();
            for (int i = 0 ; i < samplers.size() - 1 ; i++) {
                light_shader.bind_sampler(samplers[i], geometry_fbo.colors[i].view);
            }
        }

        render_target = light_fbo.colors[0].view;
        gbuffer = {
                geometry_fbo.colors[0].view,
                geometry_fbo.colors[1].view,
                geometry_fbo.colors[2].view,
                geometry_fbo.colors[3].view,
                geometry_fbo.colors[4].view,
                geometry_fbo.colors[5].view,
                geometry_fbo.colors[6].view,
                geometry_fbo.colors[7].view
        };

        set_samples(samples);
    }

    void PBR_DeferredRenderer::free() {
        geometry_shader.free();
        geometry_fbo.free();
        geometry_msaa_fbo.free();
        light_shader.free();
        light_fbo.free();
    }

    void PBR_DeferredRenderer::resize(int w, int h) {
        geometry_fbo.resize(w, h);
        geometry_msaa_fbo.resize(w, h);
        light_fbo.resize(w, h);
    }

    void PBR_DeferredRenderer::set_camera_pos(glm::vec3 &camera_pos) {
        geometry_shader.use();
        geometry_shader.set_uniform_args("camera_pos", camera_pos);

        light_shader.use();
        light_shader.set_uniform_args("camera_pos", camera_pos);
    }

    void PBR_DeferredRenderer::set_samples(int samples) {
        this->samples = samples;
        current_geometry_fbo = samples > 1 ? geometry_msaa_fbo : geometry_fbo;
    }

    PBR_Pixel PBR_DeferredRenderer::read_pixel(int x, int y) {
        PBR_Pixel pixel = geometry_fbo.read_pixel<PBR_Pixel>(5, x, y);
        return pixel;
    }

    void PBR_DeferredRenderer::bind() {
        current_geometry_fbo.bind();
        gl::clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void PBR_DeferredRenderer::unbind() {
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);

        if (samples > 1) {
            int w = geometry_fbo.rbo.width;
            int h = geometry_fbo.rbo.height;
            FrameBuffer::blit(geometry_msaa_fbo.id, w, h, geometry_fbo.id, w, h, geometry_msaa_fbo.colors.size(), GL_COLOR_BUFFER_BIT);
            FrameBuffer::blit(geometry_msaa_fbo.id, w, h, geometry_fbo.id, w, h, 1, GL_DEPTH_BUFFER_BIT);
        }

        // render SSAO
        if (enable_ssao) {
            ssao_pass->positions = gbuffer.view_position;
            ssao_pass->normals = gbuffer.view_normal;
            SSAO::render(*ssao_pass);
        }

        light_fbo.bind();
        clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        light_shader.use();

        int samplers_size = samplers.size();
        FrameBuffer current_geometry_fbo = samples > 1 ? geometry_msaa_fbo : geometry_fbo;

        light_shader.bind_sampler("direct_shadow_sampler", 0, direct_shadow_map);
        light_shader.bind_sampler("point_shadow_sampler", 1, point_shadow_map);
        light_shader.set_uniform_args("far_plane", far_plane);

        for (int i = 0 ; i < samplers_size - 1 ; i++) {
            light_shader.bind_sampler(samplers[i], current_geometry_fbo.colors[i].view);
        }

        if (enable_ssao) {
            light_shader.bind_sampler(samplers[samplers_size - 1], SSAO::render_target);
        }
        light_shader.set_uniform_args("enable_ssao", enable_ssao);

        vao.draw_quad();

        glEnable(GL_STENCIL_TEST);
        glEnable(GL_BLEND);
    }

    void PBR_DeferredRenderer::begin() {
        geometry_shader.use();
    }

    void PBR_DeferredRenderer::render(PBR_Component* pbr_component) {
        geometry_shader.set_uniform_args("object_id", pbr_component->object_id);
        pbr_component->transform.update(geometry_shader);
        pbr_component->material->update(geometry_shader, 0);
        pbr_component->drawable->draw();
    }

    void PBR_DeferredRenderer::render(PBR_Component* pbr_component, glm::mat4& light_space) {
        geometry_shader.set_uniform_args("direct_light_space", light_space);
        render(pbr_component);
    }

    void PBR_DeferredRenderer::update(Environment* env) {
        light_shader.use();
        light_shader.set_uniform_struct_args("envlight", "prefilter_levels", env->prefilter_levels);
        light_shader.bind_sampler_struct("envlight", "irradiance", 8, env->irradiance);
        light_shader.bind_sampler_struct("envlight", "prefilter", 9, env->prefilter);
        light_shader.bind_sampler_struct("envlight", "brdf_convolution", 10, env->brdf_convolution);
    }

    void PBR_DeferredRenderer::blit(int w, int h, u32 dest_fbo, int dest_object_id) {
        FrameBuffer::blit(light_fbo.id, w, h, dest_fbo, w, h, 1, GL_COLOR_BUFFER_BIT);
        FrameBuffer::blit(geometry_fbo.id, w, h, 5, dest_fbo, w, h, dest_object_id);
        FrameBuffer::blit(geometry_fbo.id, w, h, dest_fbo, w, h, 1, GL_DEPTH_BUFFER_BIT);
    }

    void PBR_Pipeline::init(int w, int h) {
        resolution = { w, h };

        env_renderer.init(w, h);

        pbr_forward_renderer.init(w, h);
        pbr_deferred_renderer.init(w, h);

        skeletal_forward_renderer.init();
        skeletal_deferred_renderer.init();

        transparent_renderer.init(pbr_forward_renderer.get_shader(), w, h);

        direct_shadow_renderer.init(w, h);
        point_shadow_renderer.init(w, h);

        outline_renderer.init();

        sunlight_ubo.init(1, sizeof(sunlight));
        lights_ubo.init(2, sizeof(point_lights));
        flashlight_ubo.init(3, sizeof(flashlight));
    }

    void PBR_Pipeline::free() {
        env_renderer.free();

        pbr_forward_renderer.free();
        pbr_deferred_renderer.free();

        skeletal_forward_renderer.free();
        skeletal_deferred_renderer.free();

        transparent_renderer.free();

        direct_shadow_renderer.free();
        point_shadow_renderer.free();

        outline_renderer.free();

        sunlight_ubo.free();
        lights_ubo.free();
        flashlight_ubo.free();
    }

    void PBR_Pipeline::set_samples(int samples) {
        pbr_forward_renderer.set_samples(samples);
        pbr_deferred_renderer.set_samples(samples);
    }

    void PBR_Pipeline::set_ssao_pass(SSAO_Pass* ssao_pass) {
        pbr_deferred_renderer.ssao_pass = ssao_pass;
    }

    void PBR_Pipeline::init_hdr_env(const char* filepath, bool flip_uv) {
        env.hdr.init(filepath, flip_uv);
    }

    void PBR_Pipeline::generate_env() {
        env_renderer.env = &env;
        env_renderer.generate_env();
        pbr_forward_renderer.update(env_renderer.env);
        pbr_deferred_renderer.update(env_renderer.env);
        skeletal_forward_renderer.update(env_renderer.env);
        skeletal_deferred_renderer.update(env_renderer.env);
    }

    void PBR_Pipeline::resize(int w, int h) {
        resolution = { w, h };

        pbr_forward_renderer.resize(w, h);
        pbr_deferred_renderer.resize(w, h);

        transparent_renderer.resize(w, h);

        direct_shadow_renderer.resize(w, h);
        point_shadow_renderer.resize(w, h);
    }

    void PBR_Pipeline::set_camera_pos(glm::vec3& camera_pos) {
        pbr_forward_renderer.set_camera_pos(camera_pos);
        pbr_deferred_renderer.set_camera_pos(camera_pos);

        skeletal_forward_renderer.set_camera_pos(camera_pos);
        skeletal_deferred_renderer.set_camera_pos(camera_pos);
    }

    PBR_Pixel PBR_Pipeline::read_pixel(int x, int y) {
        return pbr_forward_renderer.read_pixel(x, y);
    }

    void PBR_Pipeline::forward_rendering() {
        pbr_forward_renderer.bind();

        // render not face-culled objects
        glDisable(GL_CULL_FACE);
        {
            // render environment
            env_renderer.render();

            outline_renderer.end();

            // render static objects
            pbr_forward_renderer.begin();
            scene->each_component<PBR_Component_Forward>([this](PBR_Component_Forward* component) {
                pbr_forward_renderer.render(component);
            });

            // render dynamic objects
            skeletal_forward_renderer.begin();
            scene->each_component<PBR_SkeletalComponent_Forward>([this](PBR_SkeletalComponent_Forward* component) {
                skeletal_forward_renderer.render(component->object_id, component->drawable, &component->transform, component->material);
            });

            // render outline objects
            outline_renderer.begin();
            scene->each_component<Outline>([this](Outline* component) {
                outline_renderer.render(component);
            });

            outline_renderer.end();

            // render transparent objects
            transparent_renderer.begin();
            scene->each_component<PBR_Component_Transparent>([this](PBR_Component_Transparent* component) {
                transparent_renderer.render(component->object_id, component->drawable, &component->transform, component->material);
            });
            transparent_renderer.end();
        }

        // render face-culled objects
        glEnable(GL_CULL_FACE);
        {
            // render static objects
            pbr_forward_renderer.begin();
            scene->each_component<PBR_Component_ForwardCull>([this](PBR_Component_ForwardCull* component) {
                pbr_forward_renderer.render(component);
            });
            // render dynamic objects
            skeletal_forward_renderer.begin();
            scene->each_component<PBR_SkeletalComponent_ForwardCull>([this](PBR_SkeletalComponent_ForwardCull* component) {
                skeletal_forward_renderer.render(component->object_id, component->drawable, &component->transform, component->material);
            });
        }

        pbr_forward_renderer.unbind();
    }

    void PBR_Pipeline::deferred_rendering() {
        pbr_deferred_renderer.bind();

        // render not face-culled objects
        glDisable(GL_CULL_FACE);
        {
            // static objects
            pbr_deferred_renderer.begin();
            scene->each_component<PBR_Component_Deferred>([this](PBR_Component_Deferred* component) {
                pbr_deferred_renderer.render(component);
            });
            // skeletal objects
            skeletal_deferred_renderer.begin();
            scene->each_component<PBR_SkeletalComponent_Deferred>([this](PBR_SkeletalComponent_Deferred* component) {
                skeletal_deferred_renderer.render(component->object_id, component->drawable, &component->transform, component->material);
            });
        }

        // render face-culled objects
        glEnable(GL_CULL_FACE);
        {
            // static objects
            pbr_deferred_renderer.begin();
            scene->each_component<PBR_Component_DeferredCull>([this](PBR_Component_DeferredCull* component) {
                pbr_deferred_renderer.render(component);
            });
            // skeletal objects
            skeletal_deferred_renderer.begin();
            scene->each_component<PBR_SkeletalComponent_DeferredCull>([this](PBR_SkeletalComponent_DeferredCull* component) {
                skeletal_deferred_renderer.render(component->object_id, component->drawable, &component->transform, component->material);
            });
        }

        pbr_deferred_renderer.unbind();
    }

    void PBR_Pipeline::render() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glEnable(GL_BLEND);

        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glStencilMask(GL_FALSE);

        deferred_rendering();
        pbr_deferred_renderer.blit(resolution.x, resolution.y, pbr_forward_renderer.get_current_frame().id, 2);
        forward_rendering();

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);
    }

    void PBR_Pipeline::update_flashlight() {
        flashlight_ubo.update({ 0, sizeof(flashlight), &flashlight });
    }

    void PBR_Pipeline::update_sunlight() {
        sunlight_ubo.update({ 0, sizeof(sunlight), &sunlight });
    }

    void PBR_Pipeline::update_pointlights() {
        lights_ubo.update({ 0, sizeof(point_lights), point_lights.data() });
    }

}