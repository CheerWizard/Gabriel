#pragma once

#include <glm/glm.hpp>

namespace gl {

    struct LightColor final {
        glm::vec3 rgb;
        float intensity;

        LightColor(const float r = 1.0f, const float g = 1.0f, const float b = 1.0f, const float intensity = 1.0f)
        : rgb(r, g, b), intensity(intensity) {}
    };

}