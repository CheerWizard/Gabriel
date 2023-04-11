#pragma once

#include <buffers.h>
#include <shader.h>
#include <frame.h>

namespace gl {

    struct SSAO_Pass final {
        ImageBuffer positions;
        ImageBuffer normals;
        ImageBuffer noise;
        int noise_size = 4;
        glm::vec2 resolution = { 512, 512 };
        std::vector<glm::vec3> samples = std::vector<glm::vec3>(64);
        float sample_radius = 1;
        float sample_bias = 0.025;
        int occlusion_power = 2;

        void init();
        void free();
    };

    struct SSAO final {
        static ImageBuffer render_target;

        static void init(int width, int height);
        static void free();

        static void render(SSAO_Pass& pass);

        static void resize(int w, int h);

    private:
        static Shader shader;
        static Shader blur_shader;
        static FrameBuffer fbo;
        static FrameBuffer blur_fbo;
        static VertexArray vao;
    };

}