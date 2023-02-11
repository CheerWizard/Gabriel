#pragma once

#include <primitives.h>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <gtc/type_ptr.hpp>

namespace gl {

    u32 texture2d_init(
            const char* filepath,
            bool flip_uv = false,
            int param_s = GL_REPEAT,
            int param_t = GL_REPEAT,
            const glm::vec4& border_color = { 0, 0, 0, 1 },
            int min_filter = GL_LINEAR_MIPMAP_LINEAR,
            int mag_filter = GL_LINEAR
    );
    void texture_bind(u32 tbo, u32 type = GL_TEXTURE_2D, u32 slot = 0);

}