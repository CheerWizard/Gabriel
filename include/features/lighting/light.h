#pragma once

#include <api/storage.h>

#include <features/lighting/phong_light.h>
#include <features/lighting/direct_light.h>
#include <features/lighting/point_light.h>
#include <features/lighting/spot_light.h>

#include <glad/glad.h>

namespace gl {

    template<typename T>
    struct LightBuffer : StorageBuffer {
        int capacity = 1;

        LightBuffer() = default;
        LightBuffer(int capacity) : capacity(capacity) {}

        void init(u32 binding);
        void update(Scene* scene);
    };

    template<typename T>
    void LightBuffer<T>::init(u32 binding) {
        StorageBuffer::init(binding, sizeof(int) + sizeof(T) * capacity);
    }

    template<typename T>
    void LightBuffer<T>::update(Scene *scene) {
        auto& components = scene->getComponents<T>();
        long long componentsSize = components.data().size();
        void* data = components.data().data();
        StorageBuffer::bind();
        StorageBuffer::update(0, componentsSize, data);
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

}