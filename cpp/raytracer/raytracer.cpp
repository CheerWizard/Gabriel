#include <raytracer/raytracer.h>

namespace gl {

    RayTraceRenderer::RayTraceRenderer(int w, int h) {
        mShader.init("shaders/raytrace.comp");

        mOutput.width = w;
        mOutput.height = h;
        mOutput.internalFormat = GL_RGBA32F;
        mOutput.pixelFormat = GL_RGBA;
        mOutput.pixelType = PixelType::FLOAT;

        ImageParams params;
        params.minFilter = GL_NEAREST;
        params.magFilter = GL_NEAREST;
        params.s = GL_CLAMP_TO_EDGE;
        params.r = GL_CLAMP_TO_EDGE;
        params.t = GL_CLAMP_TO_EDGE;

        mRenderTarget.init();
        mRenderTarget.load(mOutput, params);
        mRenderTarget.bindImage(0, AccessMode::WRITE_ONLY, mOutput.internalFormat);

        mSphereBuffer.init(10, sizeof(Sphere));
        mRayBuffer.init(11, sizeof(Ray));

        Ray ray = { 2, 2, 2 };
        ray.direction = { 1, 1, 0, 0 };
        update(ray);

        Sphere sphere;
        sphere.center = { 0, 0, -5 };
        sphere.radius = 1.0f;
        update(sphere);
    }

    RayTraceRenderer::~RayTraceRenderer() {
        mShader.free();
        mRenderTarget.free();
        mSphereBuffer.free();
        mRayBuffer.free();
    }

    void RayTraceRenderer::render() {
        mShader.use();
        mRenderTarget.bindImage(0, AccessMode::WRITE_ONLY, mOutput.internalFormat);
        mShader.dispatch(ceil(mOutput.width / 8), ceil(mOutput.height / 8), 1);
        memoryBarrier(MemoryBarrierBits::ALL);
    }

    void RayTraceRenderer::resize(int w, int h) {
        mOutput.width = w;
        mOutput.height = h;
        mRenderTarget.bind();
        mRenderTarget.store(mOutput);
    }

    void RayTraceRenderer::update(Sphere& sphere) {
        mSphereBuffer.update(0, sizeof(sphere), &sphere);
    }

    void RayTraceRenderer::update(Ray& ray) {
        mRayBuffer.update(0, sizeof(ray), &ray);
    }

}