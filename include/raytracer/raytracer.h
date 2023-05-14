#pragma once

#include <api/shader.h>

#include <math/raycast.h>
#include <math/sphere.h>

namespace gl {

    struct RayTraceRenderer final {

        RayTraceRenderer(int w, int h);
        ~RayTraceRenderer();

        [[nodiscard]] inline const ImageBuffer& getRenderTarget() const {
            return mRenderTarget;
        }

        void resize(int w, int h);

        void render();

        void update(Ray& ray);
        void update(Sphere& sphere);

    private:
        ImageBuffer mRenderTarget;
        Image mOutput;
        ComputeShader mShader;
        UniformBuffer mSphereBuffer;
        UniformBuffer mRayBuffer;
    };

}