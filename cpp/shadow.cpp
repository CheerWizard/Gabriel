#include <shadow.h>

namespace gl {

    // directional shadows
    static gl::shadow_props direct_shadow_props;
    static u32 direct_shadow_shader;
    static u32 direct_shadow_fbo;
    static gl::depth_attachment direct_shadow;
    static gl::uniform_m4f direct_light_space = {"direct_light_space" };
    // point shadows
    static gl::shadow_props point_shadow_props;
    static u32 point_shadow_shader;
    static u32 point_shadow_fbo;
    static gl::depth_attachment point_shadow;
    static uniform_f point_shadow_far_plane = { "far_plane", 25 };
    static uniform_v3f point_shadow_light_pos = { "light_pos" };

    vertex_format shadow_vertex::format = {
            { vec3 },
            sizeof(shadow_vertex)
    };

    void direct_shadow_init(const shadow_props& props, const glm::vec3& light_dir) {
        direct_shadow_props = props;

        direct_shadow_shader = shader_init({
            "shaders/direct_shadow.vert",
            "shaders/direct_shadow.frag"
        });

        direct_shadow = { props.width, props.height };
        direct_shadow.texture.sampler = {"direct_shadow_sampler", 0 };

        direct_shadow_fbo = gl::fbo_init(null, &direct_shadow, null, null);

        direct_shadow_begin();
        direct_shadow_update(light_dir);
    }

    void point_shadow_init(const shadow_props& props, const glm::vec3& light_pos) {
        point_shadow_props = props;

        point_shadow_shader = shader_init({
            "shaders/point_shadow.vert",
            "shaders/point_shadow.frag",
            "shaders/point_shadow.geom"
        });

        point_shadow = { props.width, props.height };
        point_shadow.texture.type = GL_TEXTURE_CUBE_MAP;
        point_shadow.wrap_t = GL_CLAMP_TO_EDGE;
        point_shadow.wrap_s = GL_CLAMP_TO_EDGE;
        point_shadow.wrap_r = GL_CLAMP_TO_EDGE;
        point_shadow.texture.sampler = { "point_shadow_sampler", 1 };

        point_shadow_fbo = gl::fbo_init(null, &point_shadow, null, null);

        shader_use(point_shadow_shader);
        point_shadow_update(light_pos);
    }

    void direct_shadow_free() {
        shader_free(direct_shadow_shader);
        fbo_free(direct_shadow_fbo);
        fbo_free_attachment(direct_shadow);
    }

    void point_shadow_free() {
        shader_free(point_shadow_shader);
        fbo_free(point_shadow_fbo);
        fbo_free_attachment(point_shadow);
    }

    void direct_shadow_begin() {
        glViewport(0, 0, direct_shadow_props.width, direct_shadow_props.height);
        fbo_bind(direct_shadow_fbo);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        shader_use(direct_shadow_shader);
    }

    void point_shadow_begin() {
        glViewport(0, 0, point_shadow_props.width, point_shadow_props.height);
        fbo_bind(point_shadow_fbo);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        shader_use(point_shadow_shader);
    }

    void shadow_end(u32 fbo, int viewport_width, int viewport_height) {
        shader_use(0);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        fbo_bind(fbo);
        glViewport(0, 0, viewport_width, viewport_height);
    }

    void direct_shadow_update(const glm::vec3& light_dir) {
        float near_plane = 1.0f, far_plane = 7.5f;
        glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        glm::mat4 light_view = glm::lookAt(
                light_dir,
                glm::vec3( 0.0f, 0.0f,  0.0f),
                glm::vec3( 0.0f, 1.0f,  0.0f)
        );
        direct_light_space.value = light_projection * light_view;
        shader_set_uniform(direct_shadow_shader, direct_light_space);
    }

    void point_shadow_update(const glm::vec3& light_pos) {
        point_shadow_light_pos.value = { light_pos.x, light_pos.y, light_pos.z };

        float aspect = (float) point_shadow_props.width / (float) point_shadow_props.height;
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), aspect, 1.0f, point_shadow_far_plane.value);

        uniform_array_m4f point_light_spaces = {"light_spaces", {
                projection * glm::lookAt(light_pos, light_pos + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
                projection * glm::lookAt(light_pos, light_pos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
                projection * glm::lookAt(light_pos, light_pos + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
                projection * glm::lookAt(light_pos, light_pos + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)),
                projection * glm::lookAt(light_pos, light_pos + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)),
                projection * glm::lookAt(light_pos, light_pos + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0))
        }};

        shader_set_uniform(point_shadow_shader, point_light_spaces);
        shader_set_uniform(point_shadow_shader, point_shadow_far_plane);
        shader_set_uniform(point_shadow_shader, point_shadow_light_pos);
    }

    void direct_shadow_update(u32 shader) {
        shader_set_uniform(shader, direct_light_space);
        texture_update(shader, direct_shadow.texture);
    }

    void point_shadow_update(u32 shader) {
        shader_set_uniform(shader, point_shadow_far_plane);
        texture_update(shader, point_shadow.texture);
    }

    void direct_shadow_draw(transform& transform, const drawable_elements& drawable) {
        transform_update(direct_shadow_shader, transform);
        draw(drawable);
    }

    void point_shadow_draw(transform& transform, const drawable_elements& drawable) {
        transform_update(point_shadow_shader, transform);
        draw(drawable);
    }

    shadow_drawable_model shadow_model_init(const io::drawable_model& src) {
        shadow_drawable_model drawable;
        // invalidate model vertices and indices to group together
        drawable.model.meshes.resize(src.model.meshes.size());
        size_t mesh_count = drawable.model.meshes.size();
        u32 vertex_count = 0;
        u32 index_count = 0;
        for (u32 i = 0 ; i < mesh_count ; i++) {
            auto& src_mesh = src.model.meshes[i];
            auto& shadow_mesh = drawable.model.meshes[i];

            shadow_mesh.vertex_count = src_mesh.vertex_count;
            shadow_mesh.index_count = src_mesh.index_count;
            shadow_mesh.vertices.data = new shadow_vertex[src_mesh.vertex_count];
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

        drawable.elements.vao = vao_init();
        vao_bind(drawable.elements.vao);

        glGenBuffers(1, &drawable.elements.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, drawable.elements.vbo);
        glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(shadow_vertex), null, GL_DYNAMIC_DRAW);
        vbo_set_format(shadow_vertex::format);

        glGenBuffers(1, &drawable.elements.ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable.elements.ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(u32), null, GL_DYNAMIC_DRAW);

        u32 vertex_offset = 0;
        u32 index_offset = 0;
        for (u32 i = 0 ; i < mesh_count ; i++) {
            auto& mesh = drawable.model.meshes[i];

            glBindBuffer(GL_ARRAY_BUFFER, drawable.elements.vbo);
            glBufferSubData(GL_ARRAY_BUFFER, vertex_offset, mesh.vertex_count * sizeof(shadow_vertex), mesh.vertices.to_float());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawable.elements.ibo);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, index_offset, mesh.index_count * sizeof(u32), mesh.indices);

            vertex_offset += mesh.vertex_count * sizeof(shadow_vertex);
            index_offset += mesh.index_count * sizeof(u32);
        }

        drawable.elements.index_count = index_count;

        return drawable;
    }

    shadow_drawable_models shadow_model_init(const io::drawable_models& src) {
        shadow_drawable_models drawables;
        // todo implement
        return drawables;
    }

    void shadow_model_free(shadow_drawable_model& drawable_model) {
        for (auto& mesh : drawable_model.model.meshes) {
            delete[] mesh.vertices.data;
            delete[] mesh.indices;
        }
        drawable_free(drawable_model.elements);
    }

    void shadow_models_free(shadow_drawable_models& drawable_models) {
        for (auto& mesh : drawable_models.model.meshes) {
            delete[] mesh.vertices.data;
            delete[] mesh.indices;
        }
        for (auto& element : drawable_models.elements) {
            drawable_free(element);
        }
    }

}