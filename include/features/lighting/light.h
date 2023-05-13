#pragma once

#include <api/storage.h>

#include <features/lighting/phong_light.h>
#include <features/lighting/direct_light.h>
#include <features/lighting/point_light.h>
#include <features/lighting/spot_light.h>

#include <glad/glad.h>

namespace gl {

    template<typename T>
    struct LightBuffer : StorageBuffer<T> {

        LightBuffer() : StorageBuffer<T>() {}
        LightBuffer(long long capacity) : StorageBuffer<T>(capacity) {}

        void update(Scene* scene);
    };

    template<typename T>
    void LightBuffer<T>::update(Scene *scene) {
        auto& components = scene->getComponents<T>();
        long long componentsSize = components.data().size();
        void* data = components.data().data();
        StorageBuffer<T>::bind();
        StorageBuffer<T>::tryUpdate(0, componentsSize, data);
    }

    typedef LightBuffer<PhongLightComponent> PhongLightBuffer;
    typedef LightBuffer<DirectLightComponent> DirectLightBuffer;
    typedef LightBuffer<PointLightComponent> PointLightBuffer;
    typedef LightBuffer<SpotLightComponent> SpotLightBuffer;

    struct LightStorage final {

        static Scene* scene;

        static PhongLightBuffer phongLightBuffer;
        static DirectLightBuffer directLightBuffer;
        static PointLightBuffer pointLightBuffer;
        static SpotLightBuffer spotLightBuffer;

        static void init();
        static void free();

        static void update();

    };

    struct LightMath final {
        static float radius(const glm::vec3& color, const float brightness, const float quadratic, const float linear, const float constant);
    };

}