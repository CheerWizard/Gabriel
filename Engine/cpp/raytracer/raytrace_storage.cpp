#include <raytracer/raytrace_storage.h>

namespace gl {

    RayTraceStorage::RayTraceStorage() {
        m_sphereBuffer = 10;
        m_rayBuffer = 11;
    }

    RayTraceStorage::~RayTraceStorage() = default;

    void RayTraceStorage::update(const Ray& ray) {
        m_rayBuffer.update(ray);
    }

    void RayTraceStorage::update(const Sphere& sphere) {
        m_sphereBuffer.update(sphere);
    }

}