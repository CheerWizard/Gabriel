#pragma once

#include <glad/glad.h>

namespace gl {

    void clearDisplay(float r, float g, float b, float a, int bitmask = GL_COLOR_BUFFER_BIT);

    void polygonFill();
    void polygonLine();
    void polygonPoint();

}
