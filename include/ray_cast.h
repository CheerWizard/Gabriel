#pragma once

#include <primitives.h>

#include <glm/glm.hpp>

namespace gl {

    template<typename T>
    struct ScreenRay final {
        glm::vec3 vertex;

        void ndc_space();
        void clip_space();
        void view_space();
        void world_space();

        void to_world_ray();
    };

    template<typename T>
    void ScreenRay<T>::ndc_space() {

    }

    template<typename T>
    void ScreenRay<T>::clip_space() {

    }

    template<typename T>
    void ScreenRay<T>::view_space() {

    }

    template<typename T>
    void ScreenRay<T>::world_space() {

    }

    template<typename T>
    void ScreenRay<T>::to_world_ray() {
        ndc_space();
        clip_space();
        view_space();
        world_space();
    }

    template<typename T>
    struct WorldRay final {
        glm::vec3 vertex;

        void view_space();
        void clip_space();
        void ndc_space();
        void screen_space();

        void to_screen_ray();
    };

    template<typename T>
    void WorldRay<T>::view_space() {

    }

    template<typename T>
    void WorldRay<T>::clip_space() {

    }

    template<typename T>
    void WorldRay<T>::ndc_space() {

    }

    template<typename T>
    void WorldRay<T>::screen_space() {

    }

    template<typename T>
    void WorldRay<T>::to_screen_ray() {
        view_space();
        clip_space();
        ndc_space();
        screen_space();
    }

}