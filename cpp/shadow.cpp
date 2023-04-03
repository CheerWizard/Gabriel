#include <shadow.h>
#include <frame.h>

namespace gl {

    void DirectShadow::update() {
        float near_plane = 1.0f, far_plane = 7.5f;
        glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        glm::mat4 light_view = glm::lookAt(
                direction,
                glm::vec3( 0.0f, 0.0f,  0.0f),
                glm::vec3( 0.0f, 1.0f,  0.0f)
        );
        light_space = light_projection * light_view;
    }

    void DirectShadowRenderer::init(int w, int h) {
        shader.init(
            "shaders/direct_shadow.vert",
            "shaders/direct_shadow.frag"
        );

        fbo.depth.data = { w, h };
        fbo.depth.data.primitive_type = GL_FLOAT;
        fbo.depth.params.min_filter = GL_NEAREST;
        fbo.depth.params.mag_filter = GL_NEAREST;
        fbo.depth.params.s = GL_CLAMP_TO_EDGE;
        fbo.depth.params.t = GL_CLAMP_TO_EDGE;
        fbo.depth.params.r = GL_CLAMP_TO_EDGE;
        fbo.depth.params.border_color = { 1, 1, 1, 1 };
        fbo.depth.init();
        fbo.init();
        fbo.attach_depth();
        fbo.complete();

        render_target = fbo.depth.view;
    }

    void DirectShadowRenderer::free() {
        shader.free();
        fbo.free();
    }

    void DirectShadowRenderer::resize(int w, int h) {
        fbo.resize(w, h);
    }

    void DirectShadowRenderer::begin() {
        glViewport(0, 0, fbo.depth.data.width, fbo.depth.data.height);
        fbo.bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        shader.use();
    }

    void DirectShadowRenderer::end() {
        Shader::stop();
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    void DirectShadowRenderer::update(DirectShadow &direct_shadow) {
        direct_shadow.update();
        shader.use();
        shader.set_uniform_args("direct_light_space", direct_shadow.light_space);
    }

    void DirectShadowRenderer::render(DirectShadow& direct_shadow) {
        direct_shadow.transform.update(shader);
        direct_shadow.drawable.draw();
    }

    void PointShadow::update() {
        float aspect = (float) width / (float) height;
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), aspect, 1.0f, far_plane);
        light_spaces = {
                projection * glm::lookAt(position, position + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
                projection * glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)),
                projection * glm::lookAt(position, position + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
                projection * glm::lookAt(position, position + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)),
                projection * glm::lookAt(position, position + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)),
                projection * glm::lookAt(position, position + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0))
        };
    }

    void PointShadowRenderer::init(int w, int h) {
        shader.init(
            "shaders/point_shadow.vert",
            "shaders/point_shadow.frag",
            "shaders/point_shadow.geom"
        );

        fbo.depth.view.type = GL_TEXTURE_CUBE_MAP;
        fbo.depth.data = { w, h };
        fbo.depth.data.primitive_type = GL_FLOAT;
        fbo.depth.params.min_filter = GL_NEAREST;
        fbo.depth.params.mag_filter = GL_NEAREST;
        fbo.depth.params.s = GL_CLAMP_TO_EDGE;
        fbo.depth.params.t = GL_CLAMP_TO_EDGE;
        fbo.depth.params.r = GL_CLAMP_TO_EDGE;
        fbo.depth.params.border_color = { 1, 1, 1, 1 };
        fbo.depth.init();
        fbo.init();
        fbo.attach_depth();
        fbo.complete();

        render_target = fbo.depth.view;
    }

    void PointShadowRenderer::free() {
        shader.free();
        fbo.free();
    }

    void PointShadowRenderer::resize(int w, int h) {
        fbo.resize(w, h);
    }

    void PointShadowRenderer::begin() {
        glViewport(0, 0, fbo.depth.data.width, fbo.depth.data.height);
        fbo.bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        shader.use();
    }

    void PointShadowRenderer::end() {
        Shader::stop();
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    void PointShadowRenderer::update(PointShadow &point_shadow) {
        point_shadow.update();
        shader.use();
        UniformArrayM4F light_spaces = { "light_spaces", point_shadow.light_spaces };
        shader.set_uniform_array(light_spaces);
        shader.set_uniform_args("far_plane", point_shadow.far_plane);
        shader.set_uniform_args("light_pos", point_shadow.position);
    }

    void PointShadowRenderer::render(PointShadow& point_shadow) {
        point_shadow.transform.update(shader);
        point_shadow.drawable.draw();
    }

}