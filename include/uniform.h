#pragma once

#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include <vector>

namespace gl {

    template<typename T>
    struct uniform {
        const char* name;
        T value;
    };

    template<typename V>
    struct uniform_array {
        const char* name;
        std::vector<V> values;
    };

    typedef uniform<float> uniform_f;
    typedef uniform<int> uniform_i;
    typedef uniform<bool> uniform_b;
    typedef uniform<double> uniform_d;

    typedef uniform_array<float> uniform_array_f;
    typedef uniform_array<int> uniform_array_i;
    typedef uniform_array<bool> uniform_array_b;
    typedef uniform_array<double> uniform_array_d;

    typedef uniform<glm::fvec2> uniform_v2f;
    typedef uniform<glm::fvec3> uniform_v3f;
    typedef uniform<glm::fvec4> uniform_v4f;

    typedef uniform<glm::bvec2> uniform_v2b;
    typedef uniform<glm::bvec3> uniform_v3b;
    typedef uniform<glm::bvec4> uniform_v4b;

    typedef uniform<glm::ivec2> uniform_v2i;
    typedef uniform<glm::ivec3> uniform_v3i;
    typedef uniform<glm::ivec4> uniform_v4i;

    typedef uniform<glm::dvec2> uniform_v2d;
    typedef uniform<glm::dvec3> uniform_v3d;
    typedef uniform<glm::dvec4> uniform_v4d;

    typedef uniform_array<glm::fvec2> uniform_array_v2f;
    typedef uniform_array<glm::fvec3> uniform_array_v3f;
    typedef uniform_array<glm::fvec4> uniform_array_v4f;

    typedef uniform_array<glm::bvec2> uniform_array_v2b;
    typedef uniform_array<glm::bvec3> uniform_array_v3b;
    typedef uniform_array<glm::bvec4> uniform_array_v4b;

    typedef uniform_array<glm::ivec2> uniform_array_v2i;
    typedef uniform_array<glm::ivec3> uniform_array_v3i;
    typedef uniform_array<glm::ivec4> uniform_array_v4i;

    typedef uniform_array<glm::dvec2> uniform_array_v2d;
    typedef uniform_array<glm::dvec3> uniform_array_v3d;
    typedef uniform_array<glm::dvec4> uniform_array_v4d;

    typedef uniform<glm::fmat2> uniform_m2f;
    typedef uniform<glm::fmat3> uniform_m3f;
    typedef uniform<glm::fmat4> uniform_m4f;

    typedef uniform<glm::dmat2> uniform_m2d;
    typedef uniform<glm::dmat3> uniform_m3d;
    typedef uniform<glm::dmat4> uniform_m4d;

    typedef uniform_array<glm::fmat2> uniform_array_m2f;
    typedef uniform_array<glm::fmat3> uniform_array_m3f;
    typedef uniform_array<glm::fmat4> uniform_array_m4f;

    typedef uniform_array<glm::dmat2> uniform_array_m2d;
    typedef uniform_array<glm::dmat3> uniform_array_m3d;
    typedef uniform_array<glm::dmat4> uniform_array_m4d;

}