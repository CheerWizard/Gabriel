#include <pbr.h>
#include <array>

namespace gl {

    static std::array<ImageSampler, 6> samplers = {
            ImageSampler { "positions", 2 },
            ImageSampler { "normals", 3 },
            ImageSampler { "albedos", 4 },
            ImageSampler { "pbr_params", 5 },
            ImageSampler { "shadow_proj_coords", 6 },
            ImageSampler { "ssao", 7 }
    };

    void PBR_Skeletal_ForwardRenderer::init() {
        shader.add_vertex_stage("shaders/skeletal.vert");
        shader.add_fragment_stage("shaders/pbr.frag");
        shader.complete();
    }

    void PBR_Skeletal_ForwardRenderer::update(Environment* env) {
        shader.use();
        shader.set_uniform_struct_args("envlight", "prefilter_levels", env->prefilter_levels);
        shader.bind_sampler_struct("envlight", "irradiance", 8, env->irradiance);
        shader.bind_sampler_struct("envlight", "prefilter", 9, env->prefilter);
        shader.bind_sampler_struct("envlight", "brdf_convolution", 10, env->brdf_convolution);
    }

    void PBR_Skeletal_DeferredRenderer::init() {
        shader.add_vertex_stage("shaders/skeletal_deferred.vert");
        shader.add_fragment_stage("shaders/pbr_material.frag");
        shader.complete();
    }

    void PBR_Skeletal_DeferredRenderer::update(Environment* env) {
        shader.use();
        shader.set_uniform_struct_args("envlight", "prefilter_levels", env->prefilter_levels);
        shader.bind_sampler_struct("envlight", "irradiance", 8, env->irradiance);
        shader.bind_sampler_struct("envlight", "prefilter", 9, env->prefilter);
        shader.bind_sampler_struct("envlight", "brdf_convolution", 10, env->brdf_convolution);
    }

    void PBR_ForwardRenderer::init(int w, int h) {
        shader.add_vertex_stage("shaders/pbr.vert");
        shader.add_fragment_stage("shaders/pbr.frag");
        shader.complete();
        // setup VAO
        vao.init();
        // setup PBR color
        ColorAttachment pbr_color = {0, w, h };
        pbr_color.image.internal_format = GL_RGBA16F;
        pbr_color.image.pixel_format = GL_RGBA;
        pbr_color.image.pixel_type = PixelType::FLOAT;
        pbr_color.params.min_filter = GL_LINEAR;
        pbr_color.params.mag_filter= GL_LINEAR;
        // setup PBR object ID
        ColorAttachment pbr_entity_id = {2, w, h };
        pbr_entity_id.image.internal_format = GL_R32UI;
        pbr_entity_id.image.pixel_format = GL_RED_INTEGER;
        pbr_entity_id.image.pixel_type = PixelType::U32;
        // setup PBR frame
        fbo.colors = { pbr_color, pbr_entity_id };
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
            color.image.samples = samples;
            color.buffer.type = GL_TEXTURE_2D_MULTISAMPLE;
            color.init();
        }
        msaa_fbo.rbo.samples = samples;
        msaa_fbo.init();
        msaa_fbo.rbo.init();
        msaa_fbo.attach_colors();
        msaa_fbo.rbo.attach();
        msaa_fbo.complete();

        render_target = fbo.colors[0].buffer;

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

    void PBR_ForwardRenderer::read_pixel(PBR_Pixel& pixel, int x, int y) {
        fbo.read_pixel<PBR_Pixel>(pixel, 2, x, y);
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
            int w = msaa_fbo.colors[0].image.width;
            int h = msaa_fbo.colors[0].image.height;
            FrameBuffer::blit(msaa_fbo.id, w, h, fbo.id, w, h, 1, GL_COLOR_BUFFER_BIT);
            FrameBuffer::blit(msaa_fbo.id, w, h, fbo.id, w, h, 1, GL_DEPTH_BUFFER_BIT);
        }
    }

    void PBR_ForwardRenderer::begin() {
        shader.use();
    }

    Shader& PBR_ForwardRenderer::get_shader() {
        return shader;
    }

    FrameBuffer& PBR_ForwardRenderer::get_current_frame() {
        return current_fbo;
    }

    void PBR_ForwardRenderer::update(Environment* env) {
        shader.use();
        shader.set_uniform_struct_args("envlight", "prefilter_levels", env->prefilter_levels);
        shader.bind_sampler_struct("envlight", "irradiance", 8, env->irradiance);
        shader.bind_sampler_struct("envlight", "prefilter", 9, env->prefilter);
        shader.bind_sampler_struct("envlight", "brdf_convolution", 10, env->brdf_convolution);
    }

    void PBR_ForwardRenderer::render(ecs::EntityID entity_id, Transform& transform, DrawableElements& drawable, Material& material) {
        shader.bind_sampler("direct_shadow_sampler", 0, direct_shadow_map);
        shader.bind_sampler("point_shadow_sampler", 1, point_shadow_map);
        shader.set_uniform_args("far_plane", far_plane);
        shader.set_uniform_args("entity_id", entity_id);
        transform.update(shader);
        material.update(shader, 0);
        drawable.draw();
        transform.update(shader);
        drawable.draw();
    }

    void PBR_ForwardRenderer::render(ecs::EntityID entity_id, Transform& transform, DrawableElements& drawable, Material& material, glm::mat4& light_space) {
        shader.set_uniform_args("direct_light_space", light_space);
        render(entity_id, transform, drawable, material);
    }

    PBR_DeferredRenderer::PBR_DeferredRenderer(int w, int h) {
        ssao_renderer = new SsaoRenderer(w, h);

        geometry_shader.add_vertex_stage("shaders/pbr_material.vert");
        geometry_shader.add_fragment_stage("shaders/pbr_material.frag");
        geometry_shader.complete();

        light_shader.add_vertex_stage("shaders/fullscreen_quad.vert");
        light_shader.add_fragment_stage("shaders/pbr_light.frag");
        light_shader.complete();

        drawable.init();
        // setup PBR positions
        ColorAttachment pbr_pos = { 0, w, h };
        pbr_pos.image.internal_format = GL_RGBA32F;
        pbr_pos.image.pixel_format = GL_RGBA;
        pbr_pos.image.pixel_type = PixelType::FLOAT;
        pbr_pos.params.min_filter = GL_NEAREST;
        pbr_pos.params.mag_filter= GL_NEAREST;
        pbr_pos.params.s = GL_CLAMP_TO_EDGE;
        pbr_pos.params.t = GL_CLAMP_TO_EDGE;
        pbr_pos.params.r = GL_CLAMP_TO_EDGE;
        // setup PBR normals
        ColorAttachment pbr_normal = { 1, w, h };
        pbr_normal.image.internal_format = GL_RGBA32F;
        pbr_normal.image.pixel_format = GL_RGBA;
        pbr_normal.image.pixel_type = PixelType::FLOAT;
        pbr_normal.params.min_filter = GL_NEAREST;
        pbr_normal.params.mag_filter= GL_NEAREST;
        // setup PBR albedos
        ColorAttachment pbr_albedo = { 2, w, h };
        pbr_albedo.image.internal_format = GL_RGBA;
        pbr_albedo.image.pixel_format = GL_RGBA;
        pbr_albedo.image.pixel_type = PixelType::U8;
        pbr_albedo.params.min_filter = GL_NEAREST;
        pbr_albedo.params.mag_filter= GL_NEAREST;
        // setup PBR params
        ColorAttachment pbr_params = { 3, w, h };
        pbr_params.image.internal_format = GL_RGBA;
        pbr_params.image.pixel_format = GL_RGBA;
        pbr_params.image.pixel_type = PixelType::U8;
        pbr_params.params.min_filter = GL_NEAREST;
        pbr_params.params.mag_filter= GL_NEAREST;
        // setup PBR shadow projection coords
        ColorAttachment pbr_shadow_proj = { 4, w, h };
        pbr_shadow_proj.image.internal_format = GL_RGBA32F;
        pbr_shadow_proj.image.pixel_format = GL_RGBA;
        pbr_shadow_proj.image.pixel_type = PixelType::FLOAT;
        pbr_shadow_proj.params.min_filter = GL_NEAREST;
        pbr_shadow_proj.params.mag_filter= GL_NEAREST;
        // setup PBR object ID
        ColorAttachment pbr_entity_id = { 5, w, h };
        pbr_entity_id.image.internal_format = GL_R32UI;
        pbr_entity_id.image.pixel_format = GL_RED_INTEGER;
        pbr_entity_id.image.pixel_type = PixelType::U32;
        // setup PBR view positions
        ColorAttachment pbr_view_pos = { 6, w, h };
        pbr_view_pos.image.internal_format = GL_RGBA32F;
        pbr_view_pos.image.pixel_format = GL_RGBA;
        pbr_view_pos.image.pixel_type = PixelType::FLOAT;
        pbr_view_pos.params.min_filter = GL_NEAREST;
        pbr_view_pos.params.mag_filter= GL_NEAREST;
        pbr_view_pos.params.s = GL_CLAMP_TO_EDGE;
        pbr_view_pos.params.t = GL_CLAMP_TO_EDGE;
        pbr_view_pos.params.r = GL_CLAMP_TO_EDGE;
        // setup PBR view normals
        ColorAttachment pbr_view_normal = { 7, w, h };
        pbr_view_normal.image.internal_format = GL_RGBA32F;
        pbr_view_normal.image.pixel_format = GL_RGBA;
        pbr_view_normal.image.pixel_type = PixelType::FLOAT;
        pbr_view_normal.params.min_filter = GL_NEAREST;
        pbr_view_normal.params.mag_filter= GL_NEAREST;
        // setup PBR material frame
        geometry_fbo.colors = {
                pbr_pos,
                pbr_normal,
                pbr_albedo,
                pbr_params,
                pbr_shadow_proj,
                pbr_entity_id,
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
            color.image.samples = samples;
            color.buffer.type = GL_TEXTURE_2D_MULTISAMPLE;
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
        pbr_light_color.image.internal_format = GL_RGBA16F;
        pbr_light_color.image.pixel_format = GL_RGBA;
        pbr_light_color.image.pixel_type = PixelType::FLOAT;
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
                light_shader.bind_sampler(samplers[i], geometry_fbo.colors[i].buffer);
            }
        }

        render_target = light_fbo.colors[0].buffer;
        gbuffer = {
                geometry_fbo.colors[0].buffer,
                geometry_fbo.colors[1].buffer,
                geometry_fbo.colors[2].buffer,
                geometry_fbo.colors[3].buffer,
                geometry_fbo.colors[4].buffer,
                geometry_fbo.colors[5].buffer,
                geometry_fbo.colors[6].buffer,
                geometry_fbo.colors[7].buffer
        };

        set_samples(samples);
    }

    PBR_DeferredRenderer::~PBR_DeferredRenderer() {
        delete ssao_renderer;
        geometry_shader.free();
        geometry_fbo.free();
        geometry_msaa_fbo.free();
        light_shader.free();
        light_fbo.free();
    }

    void PBR_DeferredRenderer::resize(int w, int h) {
        ssao_renderer->resize(w, h);
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

    void PBR_DeferredRenderer::read_pixel(PBR_Pixel& pixel, int x, int y) {
        geometry_fbo.read_pixel<PBR_Pixel>(pixel, 5, x, y);
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
            ssao_renderer->get_params().positions = gbuffer.view_position;
            ssao_renderer->get_params().normals = gbuffer.view_normal;
            ssao_renderer->render();
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
            light_shader.bind_sampler(samplers[i], current_geometry_fbo.colors[i].buffer);
        }

        if (enable_ssao) {
            light_shader.bind_sampler(samplers[samplers_size - 1], ssao_renderer->get_render_target());
        }
        light_shader.set_uniform_args("enable_ssao", enable_ssao);

        drawable.draw();

        glEnable(GL_STENCIL_TEST);
        glEnable(GL_BLEND);
    }

    void PBR_DeferredRenderer::begin() {
        geometry_shader.use();
    }

    void PBR_DeferredRenderer::render(ecs::EntityID entity_id, Transform& transform, DrawableElements& drawable, Material& material) {
        geometry_shader.set_uniform_args("entity_id", entity_id);
        transform.update(geometry_shader);
        material.update(geometry_shader, 0);
        drawable.draw();
    }

    void PBR_DeferredRenderer::render(ecs::EntityID entity_id, Transform& transform, DrawableElements& drawable, Material& material, glm::mat4& light_space) {
        geometry_shader.set_uniform_args("direct_light_space", light_space);
        render(entity_id, transform, drawable, material);
    }

    void PBR_DeferredRenderer::update(Environment* env) {
        light_shader.use();
        light_shader.set_uniform_struct_args("envlight", "prefilter_levels", env->prefilter_levels);
        light_shader.bind_sampler_struct("envlight", "irradiance", 8, env->irradiance);
        light_shader.bind_sampler_struct("envlight", "prefilter", 9, env->prefilter);
        light_shader.bind_sampler_struct("envlight", "brdf_convolution", 11, env->brdf_convolution);
    }

    void PBR_ForwardRenderer::blit_color_depth(int w, int h, u32 src_color_fbo, u32 src_depth_fbo) {
        FrameBuffer::blit(src_color_fbo, w, h, current_fbo.id, w, h, 1, GL_COLOR_BUFFER_BIT);
        FrameBuffer::blit(src_depth_fbo, w, h, current_fbo.id, w, h, 1, GL_DEPTH_BUFFER_BIT);
    }

    void PBR_ForwardRenderer::blit_entity_id(int w, int h, u32 src_fbo, int src_entity_id) {
        FrameBuffer::blit(src_fbo, w, h, 5, current_fbo.id, w, h, 2);
    }

    PBR_Pipeline::PBR_Pipeline(ecs::Scene *scene, int w, int h) : scene(scene) {
        resolution = { w, h };

        env_renderer.init(w, h);

        pbr_forward_renderer.init(w, h);
        pbr_deferred_renderer = new PBR_DeferredRenderer(w, h);

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

    PBR_Pipeline::~PBR_Pipeline() {
        env_renderer.free();
        env.free();

        pbr_forward_renderer.free();
        delete pbr_deferred_renderer;

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
        pbr_deferred_renderer->set_samples(samples);
    }

    void PBR_Pipeline::init_hdr_env(const char* filepath, bool flip_uv) {
        Image hdr_image = ImageReader::read(filepath, flip_uv, PixelType::FLOAT);
        hdr_image.internal_format = GL_RGB16F;
        hdr_image.pixel_format = GL_RGB;

        ImageParams params;
        params.min_filter = GL_LINEAR;
        params.mag_filter = GL_LINEAR;
        params.s = GL_CLAMP_TO_EDGE;
        params.t = GL_CLAMP_TO_EDGE;
        params.r = GL_CLAMP_TO_EDGE;

        env.hdr.init();
        env.hdr.load(hdr_image, params);

        hdr_image.free();
    }

    void PBR_Pipeline::generate_env() {
        env_renderer.env = &env;
        env_renderer.generate_env();
        pbr_forward_renderer.update(env_renderer.env);
        pbr_deferred_renderer->update(env_renderer.env);
        skeletal_forward_renderer.update(env_renderer.env);
        skeletal_deferred_renderer.update(env_renderer.env);
    }

    void PBR_Pipeline::resize(int w, int h) {
        resolution = { w, h };

        pbr_forward_renderer.resize(w, h);
        pbr_deferred_renderer->resize(w, h);

        transparent_renderer.resize(w, h);

        direct_shadow_renderer.resize(w, h);
        point_shadow_renderer.resize(w, h);
    }

    void PBR_Pipeline::set_camera_pos(glm::vec3& camera_pos) {
        pbr_forward_renderer.set_camera_pos(camera_pos);
        pbr_deferred_renderer->set_camera_pos(camera_pos);

        skeletal_forward_renderer.set_camera_pos(camera_pos);
        skeletal_deferred_renderer.set_camera_pos(camera_pos);
    }

    void PBR_Pipeline::read_pixel(PBR_Pixel& pixel, int x, int y) {
        pbr_forward_renderer.read_pixel(pixel, x, y);
    }

    void PBR_Pipeline::forward_rendering() {
        pbr_forward_renderer.bind();

        // render face-culled objects
        forward_face_cull_render();
        // render not face-culled objects
        forward_default_render();

        pbr_forward_renderer.unbind();
    }

    void PBR_Pipeline::forward_face_cull_render() {
        glEnable(GL_CULL_FACE);
        // render static objects
        pbr_forward_renderer.begin();
        scene->each_component<PBR_Component_ForwardCull>([this](PBR_Component_ForwardCull* component) {
            ecs::EntityID entity_id = component->entity_id;
            pbr_forward_renderer.render(
                    entity_id,
                    *scene->get_component<Transform>(entity_id),
                    *scene->get_component<DrawableElements>(entity_id),
                    *scene->get_component<Material>(entity_id)
            );
        });
        // render dynamic objects
        skeletal_forward_renderer.begin();
        scene->each_component<PBR_SkeletalComponent_ForwardCull>([this](PBR_SkeletalComponent_ForwardCull* component) {
            ecs::EntityID entity_id = component->entity_id;
            skeletal_forward_renderer.render(
                    entity_id,
                    *scene->get_component<Transform>(entity_id),
                    *scene->get_component<DrawableElements>(entity_id),
                    *scene->get_component<Material>(entity_id)
            );
        });
    }

    void PBR_Pipeline::forward_default_render() {
        glDisable(GL_CULL_FACE);
        // render environment
        env_renderer.render();

        outline_renderer.end();

        // render static objects
        pbr_forward_renderer.begin();
        scene->each_component<PBR_Component_Forward>([this](PBR_Component_Forward* component) {
            ecs::EntityID entity_id = component->entity_id;
            pbr_forward_renderer.render(
                    entity_id,
                    *scene->get_component<Transform>(entity_id),
                    *scene->get_component<DrawableElements>(entity_id),
                    *scene->get_component<Material>(entity_id)
            );
        });

        // render dynamic objects
        skeletal_forward_renderer.begin();
        scene->each_component<PBR_SkeletalComponent_Forward>([this](PBR_SkeletalComponent_Forward* component) {
            ecs::EntityID entity_id = component->entity_id;
            skeletal_forward_renderer.render(
                    entity_id,
                    *scene->get_component<Transform>(entity_id),
                    *scene->get_component<DrawableElements>(entity_id),
                    *scene->get_component<Material>(entity_id)
            );
        });

        // render outline objects
        outline_renderer.begin();
        scene->each_component<Outline>([this](Outline* component) {
            ecs::EntityID entity_id = component->entity_id;
            outline_renderer.render(
                    *component,
                    *scene->get_component<Transform>(entity_id),
                    *scene->get_component<DrawableElements>(entity_id)
            );
        });
        outline_renderer.end();

        // render transparent objects
        transparent_renderer.begin();
        scene->each_component<PBR_Component_Transparent>([this](PBR_Component_Transparent* component) {
            ecs::EntityID entity_id = component->entity_id;
            transparent_renderer.render(
                    entity_id,
                    *scene->get_component<Transform>(entity_id),
                    *scene->get_component<DrawableElements>(entity_id),
                    *scene->get_component<Material>(entity_id)
            );
        });
        transparent_renderer.end();
    }

    void PBR_Pipeline::deferred_rendering() {
        pbr_deferred_renderer->bind();

        // render not face-culled objects
        glDisable(GL_CULL_FACE);
        {
            // static objects
            pbr_deferred_renderer->begin();
            scene->each_component<PBR_Component_Deferred>([this](PBR_Component_Deferred* component) {
                ecs::EntityID entity_id = component->entity_id;
                pbr_deferred_renderer->render(
                        entity_id,
                        *scene->get_component<Transform>(entity_id),
                        *scene->get_component<DrawableElements>(entity_id),
                        *scene->get_component<Material>(entity_id)
                );
            });
            // skeletal objects
            skeletal_deferred_renderer.begin();
            scene->each_component<PBR_SkeletalComponent_Deferred>([this](PBR_SkeletalComponent_Deferred* component) {
                ecs::EntityID entity_id = component->entity_id;
                skeletal_deferred_renderer.render(
                        entity_id,
                        *scene->get_component<Transform>(entity_id),
                        *scene->get_component<DrawableElements>(entity_id),
                        *scene->get_component<Material>(entity_id)
                );
            });
        }

        // render face-culled objects
        glEnable(GL_CULL_FACE);
        {
            // static objects
            pbr_deferred_renderer->begin();
            scene->each_component<PBR_Component_DeferredCull>([this](PBR_Component_DeferredCull* component) {
                ecs::EntityID entity_id = component->entity_id;
                pbr_deferred_renderer->render(
                        entity_id,
                        *scene->get_component<Transform>(entity_id),
                        *scene->get_component<DrawableElements>(entity_id),
                        *scene->get_component<Material>(entity_id)
                );
            });
            // skeletal objects
            skeletal_deferred_renderer.begin();
            scene->each_component<PBR_SkeletalComponent_DeferredCull>([this](PBR_SkeletalComponent_DeferredCull* component) {
                ecs::EntityID entity_id = component->entity_id;
                skeletal_deferred_renderer.render(
                        entity_id,
                        *scene->get_component<Transform>(entity_id),
                        *scene->get_component<DrawableElements>(entity_id),
                        *scene->get_component<Material>(entity_id)
                );
            });
        }

        pbr_deferred_renderer->unbind();
    }

    void PBR_Pipeline::render() {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glEnable(GL_BLEND);

        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glStencilMask(GL_FALSE);

        deferred_rendering();
        pbr_forward_renderer.blit_color_depth(
                resolution.x, resolution.y,
                pbr_deferred_renderer->get_light_fbo().id,
                pbr_deferred_renderer->get_geometry_fbo().id
        );
        forward_rendering();
        pbr_forward_renderer.blit_entity_id(
            resolution.x,
            resolution.y,
            pbr_deferred_renderer->get_geometry_fbo().id,
            5
        );

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