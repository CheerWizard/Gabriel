#include <raytracer/raytrace_renderer.h>

namespace gl {

    RayTraceRenderer::RayTraceRenderer(int w, int h) {
        m_shader.init("shaders/raytrace.comp");

        m_output.width = w;
        m_output.height = h;
        m_output.internalFormat = GL_RGBA32F;
        m_output.pixelFormat = GL_RGBA;
        m_output.pixelType = PixelType::FLOAT;

        ImageParams params;
        params.minFilter = GL_NEAREST;
        params.magFilter = GL_NEAREST;
        params.s = GL_CLAMP_TO_EDGE;
        params.r = GL_CLAMP_TO_EDGE;
        params.t = GL_CLAMP_TO_EDGE;

        m_renderTarget.init();
        m_renderTarget.load(m_output, params);
        m_renderTarget.bindImage(0, AccessMode::WRITE_ONLY, m_output.internalFormat);
    }

    RayTraceRenderer::~RayTraceRenderer() {
        m_shader.free();
        m_renderTarget.free();
    }

    void RayTraceRenderer::render() {
        m_shader.use();
        m_renderTarget.bindImage(0, AccessMode::WRITE_ONLY, m_output.internalFormat);
        m_shader.dispatch(ceil(m_output.width / 8), ceil(m_output.height / 8), 1);
        memoryBarrier(MemoryBarrierBits::ALL);
    }

    void RayTraceRenderer::resize(int w, int h) {
        m_output.width = w;
        m_output.height = h;
        m_renderTarget.bind();
        m_renderTarget.store(m_output);
    }

}