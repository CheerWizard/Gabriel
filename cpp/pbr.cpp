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

    PBR_ForwardRenderer::PBR_ForwardRenderer(int width, int height) {
        mShader.add_vertex_stage("shaders/pbr.vert");
        mShader.add_fragment_stage("shaders/pbr.frag");
        mShader.complete();
        // setup VAO
        vao.init();
        // setup PBR color
        ColorAttachment pbr_color = {0, width, height };
        pbr_color.image.internal_format = GL_RGBA16F;
        pbr_color.image.pixel_format = GL_RGBA;
        pbr_color.image.pixel_type = PixelType::FLOAT;
        pbr_color.params.min_filter = GL_LINEAR;
        pbr_color.params.mag_filter= GL_LINEAR;
        // setup PBR object ID
        ColorAttachment pbr_entity_id = {2, width, height };
        pbr_entity_id.image.internal_format = GL_R32UI;
        pbr_entity_id.image.pixel_format = GL_RED_INTEGER;
        pbr_entity_id.image.pixel_type = PixelType::U32;
        // setup PBR frame
        fbo.colors = { pbr_color, pbr_entity_id };
        fbo.init_colors();
        fbo.rbo = {width, height };
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

    PBR_ForwardRenderer::~PBR_ForwardRenderer() {
        vao.free();
        mShader.free();
        fbo.free();
        msaa_fbo.free();
    }

    void PBR_ForwardRenderer::resize(int w, int h) {
        fbo.resize(w, h);
        msaa_fbo.resize(w, h);
    }

    void PBR_ForwardRenderer::set_camera_pos(glm::vec3 &camera_pos) {
        mShader.use();
        mShader.set_uniform_args("camera_pos", camera_pos);
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
        mShader.use();
    }

    Shader& PBR_ForwardRenderer::get_shader() {
        return mShader;
    }

    FrameBuffer& PBR_ForwardRenderer::get_current_frame() {
        return current_fbo;
    }

    void PBR_ForwardRenderer::update(Environment* env) {
        mShader.use();
        mShader.set_uniform_struct_args("envlight", "prefilter_levels", env->prefilter_levels);
        mShader.bind_sampler_struct("envlight", "irradiance", 8, env->irradiance);
        mShader.bind_sampler_struct("envlight", "prefilter", 9, env->prefilter);
        mShader.bind_sampler_struct("envlight", "brdf_convolution", 10, env->brdf_convolution);
    }

    void PBR_ForwardRenderer::render(ecs::EntityID entity_id, Transform& transform, DrawableElements& drawable, Material& material) {
        mShader.set_uniform_args("entity_id", entity_id);

        if (!directShadow->lightSpaces.empty()) {
            mShader.bind_sampler("direct_shadow_sampler", 0, directShadow->map.buffer);
            mShader.set_uniform_args("direct_light_space", directShadow->lightSpaces[0]);
        }

        mShader.bind_sampler("point_shadow_sampler", 1, pointShadow->map.buffer);
        mShader.set_uniform_args("far_plane", pointShadow->zFar);

        transform.update(mShader);
        material.update(mShader, 0);
        drawable.draw();
    }

    void PBR_ForwardRenderer::render(ecs::EntityID entity_id, Transform& transform, DrawableElements& drawable, Material& material, glm::mat4& light_space) {
        render(entity_id, transform, drawable, material);
    }

    PBR_DeferredRenderer::PBR_DeferredRenderer(int w, int h) {
        ssao_renderer = new SsaoRenderer(w, h);

        mGeometryShader.add_vertex_stage("shaders/pbr_material.vert");
        mGeometryShader.add_fragment_stage("shaders/pbr_material.frag");
        mGeometryShader.complete();

        mLightShader.add_vertex_stage("shaders/fullscreen_quad.vert");
        mLightShader.add_fragment_stage("shaders/pbr_light.frag");
        mLightShader.complete();

        mDrawable.init();
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
        mGeometryFrame.colors = {
                pbr_pos,
                pbr_normal,
                pbr_albedo,
                pbr_params,
                pbr_shadow_proj,
                pbr_entity_id,
                pbr_view_pos,
                pbr_view_normal
        };
        mGeometryFrame.init_colors();
        mGeometryFrame.rbo = {w, h };
        mGeometryFrame.rbo.init();
        mGeometryFrame.init();
        mGeometryFrame.attach_colors();
        mGeometryFrame.attach_render_buffer();
        mGeometryFrame.complete();
        // setup PBR material MSAA frame
        mGeometryMsaaFrame = mGeometryFrame;
        for (auto& color : mGeometryMsaaFrame.colors) {
            color.image.samples = samples;
            color.buffer.type = GL_TEXTURE_2D_MULTISAMPLE;
            color.init();
        }
        mGeometryMsaaFrame.rbo.samples = samples;
        mGeometryMsaaFrame.rbo.init();
        mGeometryMsaaFrame.init();
        mGeometryMsaaFrame.attach_colors();
        mGeometryMsaaFrame.attach_render_buffer();
        mGeometryMsaaFrame.complete();
        // setup PBR light color
        ColorAttachment pbr_light_color = { 0, w, h };
        pbr_light_color.image.internal_format = GL_RGBA16F;
        pbr_light_color.image.pixel_format = GL_RGBA;
        pbr_light_color.image.pixel_type = PixelType::FLOAT;
        pbr_light_color.params.min_filter = GL_LINEAR;
        pbr_light_color.params.mag_filter= GL_LINEAR;
        // setup PBR light frame
        mLightFrame.colors = {pbr_light_color };
        mLightFrame.init_colors();
        mLightFrame.rbo = {w, h };
        mLightFrame.rbo.init();
        mLightFrame.init();
        mLightFrame.attach_colors();
        mLightFrame.attach_render_buffer();
        mLightFrame.complete();
        // PBR light pass upload
        {
            mLightShader.use();
            for (int i = 0 ; i < samplers.size() - 1 ; i++) {
                mLightShader.bind_sampler(samplers[i], mGeometryFrame.colors[i].buffer);
            }
        }

        render_target = mLightFrame.colors[0].buffer;
        gbuffer = {
                mGeometryFrame.colors[0].buffer,
                mGeometryFrame.colors[1].buffer,
                mGeometryFrame.colors[2].buffer,
                mGeometryFrame.colors[3].buffer,
                mGeometryFrame.colors[4].buffer,
                mGeometryFrame.colors[5].buffer,
                mGeometryFrame.colors[6].buffer,
                mGeometryFrame.colors[7].buffer
        };

        set_samples(samples);
    }

    PBR_DeferredRenderer::~PBR_DeferredRenderer() {
        delete ssao_renderer;
        mGeometryShader.free();
        mGeometryFrame.free();
        mGeometryMsaaFrame.free();
        mLightShader.free();
        mLightFrame.free();
    }

    void PBR_DeferredRenderer::resize(int w, int h) {
        ssao_renderer->resize(w, h);
        mGeometryFrame.resize(w, h);
        mGeometryMsaaFrame.resize(w, h);
        mLightFrame.resize(w, h);
    }

    void PBR_DeferredRenderer::set_camera_pos(glm::vec3 &camera_pos) {
        mGeometryShader.use();
        mGeometryShader.set_uniform_args("camera_pos", camera_pos);

        mLightShader.use();
        mLightShader.set_uniform_args("camera_pos", camera_pos);
    }

    void PBR_DeferredRenderer::set_samples(int samples) {
        this->samples = samples;
        mCurrentGeometryFrame = samples > 1 ? mGeometryMsaaFrame : mGeometryFrame;
    }

    void PBR_DeferredRenderer::read_pixel(PBR_Pixel& pixel, int x, int y) {
        mGeometryFrame.read_pixel<PBR_Pixel>(pixel, 5, x, y);
    }

    void PBR_DeferredRenderer::bind() {
        mCurrentGeometryFrame.bind();
        gl::clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void PBR_DeferredRenderer::unbind() {
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);

        if (samples > 1) {
            int w = mGeometryFrame.rbo.width;
            int h = mGeometryFrame.rbo.height;
            FrameBuffer::blit(mGeometryMsaaFrame.id, w, h, mGeometryFrame.id, w, h, mGeometryMsaaFrame.colors.size(), GL_COLOR_BUFFER_BIT);
            FrameBuffer::blit(mGeometryMsaaFrame.id, w, h, mGeometryFrame.id, w, h, 1, GL_DEPTH_BUFFER_BIT);
        }

        // render SSAO
        if (enable_ssao) {
            ssao_renderer->get_params().positions = gbuffer.view_position;
            ssao_renderer->get_params().normals = gbuffer.view_normal;
            ssao_renderer->render();
        }

        mLightFrame.bind();
        clear_display(COLOR_CLEAR, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mLightShader.use();

        int samplers_size = samplers.size();
        FrameBuffer current_geometry_fbo = samples > 1 ? mGeometryMsaaFrame : mGeometryFrame;

        mLightShader.bind_sampler("direct_shadow_sampler", 0, directShadow->map.buffer);
        mLightShader.bind_sampler("point_shadow_sampler", 1, pointShadow->map.buffer);
        mLightShader.set_uniform_args("far_plane", pointShadow->zFar);

        for (int i = 0 ; i < samplers_size - 1 ; i++) {
            mLightShader.bind_sampler(samplers[i], current_geometry_fbo.colors[i].buffer);
        }

        if (enable_ssao) {
            mLightShader.bind_sampler(samplers[samplers_size - 1], ssao_renderer->get_render_target());
        }
        mLightShader.set_uniform_args("enable_ssao", enable_ssao);

        mDrawable.draw();

        glEnable(GL_STENCIL_TEST);
        glEnable(GL_BLEND);
    }

    void PBR_DeferredRenderer::begin() {
        mGeometryShader.use();
    }

    void PBR_DeferredRenderer::render(ecs::EntityID entity_id, Transform& transform, DrawableElements& drawable, Material& material) {
        mGeometryShader.set_uniform_args("entity_id", entity_id);

        if (!directShadow->lightSpaces.empty()) {
            mGeometryShader.set_uniform_args("direct_light_space", directShadow->lightSpaces[0]);
        }

        transform.update(mGeometryShader);
        material.update(mGeometryShader, 0);
        drawable.draw();
    }

    void PBR_DeferredRenderer::update(Environment* env) {
        mLightShader.use();
        mLightShader.set_uniform_struct_args("envlight", "prefilter_levels", env->prefilter_levels);
        mLightShader.bind_sampler_struct("envlight", "irradiance", 8, env->irradiance);
        mLightShader.bind_sampler_struct("envlight", "prefilter", 9, env->prefilter);
        mLightShader.bind_sampler_struct("envlight", "brdf_convolution", 11, env->brdf_convolution);
    }

    void PBR_ForwardRenderer::blit_color_depth(int w, int h, u32 src_color_fbo, u32 src_depth_fbo) {
        FrameBuffer::blit(src_color_fbo, w, h, current_fbo.id, w, h, 1, GL_COLOR_BUFFER_BIT);
        FrameBuffer::blit(src_depth_fbo, w, h, current_fbo.id, w, h, 1, GL_DEPTH_BUFFER_BIT);
    }

    void PBR_ForwardRenderer::blit_entity_id(int w, int h, u32 src_fbo, int src_entity_id) {
        FrameBuffer::blit(src_fbo, w, h, 5, current_fbo.id, w, h, 2);
    }

    PBR_Pipeline::PBR_Pipeline(ecs::Scene *scene, int width, int height) : scene(scene) {
        resolution = { width, height };

        env_renderer = new EnvRenderer(width, height);

        pbr_forward_renderer = new PBR_ForwardRenderer(width, height);
        pbr_deferred_renderer = new PBR_DeferredRenderer(width, height);

        skeletal_forward_renderer.init();
        skeletal_deferred_renderer.init();

        transparent_renderer.init(pbr_forward_renderer->get_shader(), width, height);

        outline_renderer.init();

        sunlight_ubo.init(1, sizeof(DirectLightUniform));
        lights_ubo.init(2, sizeof(PointLightUniform) * 4);
        flashlight_ubo.init(3, sizeof(SpotLightUniform));
    }

    PBR_Pipeline::~PBR_Pipeline() {
        delete env_renderer;
        env.free();

        delete pbr_forward_renderer;
        delete pbr_deferred_renderer;

        skeletal_forward_renderer.free();
        skeletal_deferred_renderer.free();

        transparent_renderer.free();

        outline_renderer.free();

        sunlight_ubo.free();
        lights_ubo.free();
        flashlight_ubo.free();
    }

    void PBR_Pipeline::set_samples(int samples) {
        pbr_forward_renderer->set_samples(samples);
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
        env_renderer->environment = &env;
        env_renderer->generate();
        pbr_forward_renderer->update(env_renderer->environment);
        pbr_deferred_renderer->update(env_renderer->environment);
        skeletal_forward_renderer.update(env_renderer->environment);
        skeletal_deferred_renderer.update(env_renderer->environment);
    }

    void PBR_Pipeline::resize(int width, int height) {
        resolution = {width, height };

        pbr_forward_renderer->resize(width, height);
        pbr_deferred_renderer->resize(width, height);

        transparent_renderer.resize(width, height);
    }

    void PBR_Pipeline::set_camera_pos(glm::vec3& camera_pos) {
        pbr_forward_renderer->set_camera_pos(camera_pos);
        pbr_deferred_renderer->set_camera_pos(camera_pos);

        skeletal_forward_renderer.set_camera_pos(camera_pos);
        skeletal_deferred_renderer.set_camera_pos(camera_pos);
    }

    void PBR_Pipeline::read_pixel(PBR_Pixel& pixel, int x, int y) {
        pbr_forward_renderer->read_pixel(pixel, x, y);
    }

    void PBR_Pipeline::forward_rendering() {
        pbr_forward_renderer->bind();

        // render face-culled objects
        forward_face_cull_render();
        // render not face-culled objects
        forward_default_render();

        pbr_forward_renderer->unbind();
    }

    void PBR_Pipeline::forward_face_cull_render() {
        glEnable(GL_CULL_FACE);
        // render static objects
        pbr_forward_renderer->begin();
        scene->each_component<PBR_Component_ForwardCull>([this](PBR_Component_ForwardCull* component) {
            ecs::EntityID entity_id = component->entity_id;
            pbr_forward_renderer->render(
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
        env_renderer->render();

        outline_renderer.end();

        // render static objects
        pbr_forward_renderer->begin();
        scene->each_component<PBR_Component_Forward>([this](PBR_Component_Forward* component) {
            ecs::EntityID entity_id = component->entity_id;
            pbr_forward_renderer->render(
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
            // terrain
            if (terrain) {
                glCullFace(GL_FRONT);
                pbr_deferred_renderer->render(
                        terrain->entityId,
                        terrain->transform,
                        terrain->drawable,
                        terrain->material
                );
                glCullFace(GL_BACK);
            }
            // world objects
            scene->each_component<PBR_Component_DeferredCull>([this](PBR_Component_DeferredCull* component) {
                ecs::EntityID entity_id = component->entity_id;
                pbr_deferred_renderer->render(
                        entity_id,
                        *scene->get_component<Transform>(entity_id),
                        *scene->get_component<DrawableElements>(entity_id),
                        *scene->get_component<Material>(entity_id)
                );
            });
            // skeletal world objects
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
        pbr_forward_renderer->blit_color_depth(
                resolution.x, resolution.y,
                pbr_deferred_renderer->get_light_fbo().id,
                pbr_deferred_renderer->get_geometry_fbo().id
        );
        forward_rendering();
        pbr_forward_renderer->blit_entity_id(
            resolution.x,
            resolution.y,
            pbr_deferred_renderer->get_geometry_fbo().id,
            5
        );

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);
    }

    void PBR_Pipeline::updateSunlight(DirectLightUniform& sunlight) {
        sunlight_ubo.update({ 0, sizeof(sunlight), &sunlight });
    }

    void PBR_Pipeline::updatePointLights(std::array<PointLightUniform, 4>& pointLights) {
        lights_ubo.update({ 0, sizeof(pointLights), pointLights.data() });
    }

    void PBR_Pipeline::updateFlashlight(SpotLightUniform& flashlight) {
        flashlight_ubo.update({ 0, sizeof(flashlight), &flashlight });
    }

}