#pragma once

#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include <vector>

namespace gl {

    template<typename T>
    struct Uniform {
        const char* name;
        T value;
    };

    template<typename V>
    struct UniformArray {
        const char* name;
        std::vector<V> values;
    };

    typedef Uniform<float> UniformF;
    typedef Uniform<int> UniformI;
    typedef Uniform<bool> UniformB;
    typedef Uniform<double> UniformD;

    typedef UniformArray<float> UniformArrayF;
    typedef UniformArray<int> UniformArrayI;
    typedef UniformArray<bool> UniformArrayB;
    typedef UniformArray<double> UniformArrayD;

    typedef Uniform<glm::fvec2> UniformV2F;
    typedef Uniform<glm::fvec3> UniformV3F;
    typedef Uniform<glm::fvec4> UniformV4F;

    typedef Uniform<glm::bvec2> UniformV2B;
    typedef Uniform<glm::bvec3> UniformV3B;
    typedef Uniform<glm::bvec4> UniformV4B;

    typedef Uniform<glm::ivec2> UniformV2I;
    typedef Uniform<glm::ivec3> UniformV3I;
    typedef Uniform<glm::ivec4> UniformV4I;

    typedef Uniform<glm::dvec2> UniformV2D;
    typedef Uniform<glm::dvec3> UniformV3D;
    typedef Uniform<glm::dvec4> UniformV4D;

    typedef UniformArray<glm::fvec2> UniformArrayV2F;
    typedef UniformArray<glm::fvec3> UniformArrayV3F;
    typedef UniformArray<glm::fvec4> UniformArrayV4F;

    typedef UniformArray<glm::bvec2> UniformArrayV2B;
    typedef UniformArray<glm::bvec3> UniformArrayV3B;
    typedef UniformArray<glm::bvec4> UniformArrayV4B;

    typedef UniformArray<glm::ivec2> UniformArrayV2I;
    typedef UniformArray<glm::ivec3> UniformArrayV3I;
    typedef UniformArray<glm::ivec4> UniformArrayV4I;

    typedef UniformArray<glm::dvec2> UniformArrayV2D;
    typedef UniformArray<glm::dvec3> UniformArrayV3D;
    typedef UniformArray<glm::dvec4> UniformArrayV4D;

    typedef Uniform<glm::fmat2> UniformM2F;
    typedef Uniform<glm::fmat3> UniformM3F;
    typedef Uniform<glm::fmat4> UniformM4F;

    typedef Uniform<glm::dmat2> UniformM2D;
    typedef Uniform<glm::dmat3> UniformM3D;
    typedef Uniform<glm::dmat4> UniformM4D;

    typedef UniformArray<glm::fmat2> UniformArrayM2F;
    typedef UniformArray<glm::fmat3> UniformArrayM3F;
    typedef UniformArray<glm::fmat4> UniformArrayM4F;

    typedef UniformArray<glm::dmat2> UniformArrayM2D;
    typedef UniformArray<glm::dmat3> UniformArrayM3D;
    typedef UniformArray<glm::dmat4> UniformArrayM4D;

}