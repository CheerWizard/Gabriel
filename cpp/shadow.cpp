#include <shadow.h>

namespace gl {

    // directional shadows
    static Shader direct_shadow_shader;
    static FrameBuffer direct_shadow_fbo;
    // point shadows
    static Shader point_shadow_shader;
    static FrameBuffer point_shadow_fbo;

    VertexFormat ShadowVertex::format = {
            { vec3 },
            sizeof(ShadowVertex)
    };

    void DirectShadow::init() {
        direct_shadow_shader.init(
            "shaders/direct_shadow.vert",
            "shaders/direct_shadow.frag"
        );

        direct_shadow_fbo.depth.data = { width, height };
        direct_shadow_fbo.depth.view.sampler = {"direct_shadow_sampler", 0 };
        direct_shadow_fbo.flags = init_depth;
        direct_shadow_fbo.init();

        direct_shadow_shader.use();
        update();
    }

    void PointShadow::init() {
        point_shadow_shader.init(
            "shaders/point_shadow.vert",
            "shaders/point_shadow.frag",
            "shaders/point_shadow.geom"
        );

        point_shadow_fbo.depth.data = { width, height };
        point_shadow_fbo.depth.view.type = GL_TEXTURE_CUBE_MAP;
        point_shadow_fbo.depth.params.s = GL_CLAMP_TO_EDGE;
        point_shadow_fbo.depth.params.t = GL_CLAMP_TO_EDGE;
        point_shadow_fbo.depth.params.r = GL_CLAMP_TO_EDGE;
        point_shadow_fbo.depth.view.sampler = { "point_shadow_sampler", 1 };
        point_shadow_fbo.flags = init_depth;
        point_shadow_fbo.init();

        point_shadow_shader.use();
        update();
    }

    void DirectShadow::free() {
        direct_shadow_shader.free();
        direct_shadow_fbo.free();
    }

    void PointShadow::free() {
        point_shadow_shader.free();
        point_shadow_fbo.free();
    }

    void DirectShadow::begin() const {
        glViewport(0, 0, width, height);
        direct_shadow_fbo.bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        direct_shadow_shader.use();
    }

    void PointShadow::begin() const {
        glViewport(0, 0, width, height);
        point_shadow_fbo.bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        point_shadow_shader.use();
    }

    void DirectShadow::end() {
        glUseProgram(0);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    void PointShadow::end() {
        glUseProgram(0);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    void DirectShadow::update() {
        float near_plane = 1.0f, far_plane = 7.5f;
        glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        glm::mat4 light_view = glm::lookAt(
                direction,
                glm::vec3( 0.0f, 0.0f,  0.0f),
                glm::vec3( 0.0f, 1.0f,  0.0f)
        );
        light_space = light_projection * light_view;
        direct_shadow_shader.set_uniform_args("direct_light_space", light_space);
    }

    void PointShadow::update() {
        float aspect = (float) width / (float) height;
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), aspect, 1.0f, far_plane);

        UniformArrayM4F light_spaces = { "light_spaces", {
                projection * glm::lookAt(position, position + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
                projection * glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
                projection * glm::lookAt(position, position + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
                projection * glm::lookAt(position, position + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)),
                projection * glm::lookAt(position, position + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)),
                projection * glm::lookAt(position, position + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0))
        }};

        point_shadow_shader.set_uniform_array(light_spaces);
        point_shadow_shader.set_uniform_args<float>("far_plane", far_plane);
        point_shadow_shader.set_uniform_args<glm::vec3>("light_pos", position);
    }

    void DirectShadow::update(Shader& shader) {
        shader.set_uniform_args("direct_light_space", light_space);
        direct_shadow_fbo.depth.view.update(shader);
    }

    void PointShadow::update(Shader& shader) {
        shader.set_uniform_args<float>("far_plane", far_plane);
        point_shadow_fbo.depth.view.update(shader);
    }

    void DirectShadow::draw(Transform &transform, const DrawableElements &drawable) {
        transform.update(direct_shadow_shader);
        drawable.draw();
    }

    void PointShadow::draw(Transform &transform, const DrawableElements &drawable) {
        transform.update(point_shadow_shader);
        drawable.draw();
    }

    void ShadowDrawableModel::init(const io::DrawableModel& src) {
        // invalidate Model vertices and indices to group together
        model.meshes.resize(src.model.meshes.size());
        size_t mesh_count = model.meshes.size();
        u32 vertex_count = 0;
        u32 index_count = 0;
        for (u32 i = 0 ; i < mesh_count ; i++) {
            auto& src_mesh = src.model.meshes[i];
            auto& shadow_mesh = model.meshes[i];

            shadow_mesh.vertex_count = src_mesh.vertex_count;
            shadow_mesh.index_count = src_mesh.index_count;
            shadow_mesh.vertices.data = new ShadowVertex[src_mesh.vertex_count];
            shadow_mesh.indices = new u32[src_mesh.index_count];

            for (u32 j = 0 ; j < src_mesh.vertex_count ; j++) {
                shadow_mesh.vertices.data[j].pos = src_mesh.vertices.data[j].pos;
            }

            for (u32 j = 0 ; j < src_mesh.index_count ; j++) {
                shadow_mesh.indices[j] = src_mesh.indices[j];
            }

            vertex_count += src_mesh.vertex_count;
            index_count += src_mesh.index_count;
        }

        elements.vao.init();
        elements.vao.bind();

        glGenBuffers(1, &elements.vbo.id);
        glBindBuffer(GL_ARRAY_BUFFER, elements.vbo.id);
        glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(ShadowVertex), null, GL_DYNAMIC_DRAW);
        elements.vbo.set_format(ShadowVertex::format);

        glGenBuffers(1, &elements.ibo.id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements.ibo.id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(u32), null, GL_DYNAMIC_DRAW);

        u32 vertex_offset = 0;
        u32 index_offset = 0;
        for (u32 i = 0 ; i < mesh_count ; i++) {
            auto& mesh = model.meshes[i];

            glBindBuffer(GL_ARRAY_BUFFER, elements.vbo.id);
            glBufferSubData(GL_ARRAY_BUFFER, vertex_offset, mesh.vertex_count * sizeof(ShadowVertex), mesh.vertices.to_float());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements.ibo.id);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, index_offset, mesh.index_count * sizeof(u32), mesh.indices);

            vertex_offset += mesh.vertex_count * sizeof(ShadowVertex);
            index_offset += mesh.index_count * sizeof(u32);
        }

        elements.index_count = index_count;
    }

    void ShadowDrawableModels::init(const io::DrawableModels& src) {
        // todo implement
    }

    void ShadowDrawableModel::free() {
        for (auto& mesh : model.meshes) {
            delete[] mesh.vertices.data;
            delete[] mesh.indices;
        }
        elements.free();
    }

    void ShadowDrawableModels::free() {
        for (auto& mesh : model.meshes) {
            delete[] mesh.vertices.data;
            delete[] mesh.indices;
        }
        for (auto& element : elements) {
            element.free();
        }
    }

}