#pragma once

#include <api/uniform.h>

namespace gl {

    typedef DataBuffer<Ray> RayBuffer;
    typedef DataBuffer<Sphere> SphereBuffer;

    struct GABRIEL_API RayTraceStorage final {

        static RayTraceStorage& get() {
            static RayTraceStorage instance;
            return instance;
        }

        void update(const Ray& ray);
        void update(const Sphere& sphere);

    private:
        RayTraceStorage();
        ~RayTraceStorage();

    private:
        RayBuffer m_rayBuffer;
        SphereBuffer m_sphereBuffer;
    };

}