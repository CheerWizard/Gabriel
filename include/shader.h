#pragma once

#include <primitives.h>

namespace gl {

    u32 shader_init(const char* filepath, u32 shader_type);

    struct shader_program_props {
        const char* vertex_filepath;
        const char* fragment_filepath;
    };

    u32 shader_program_init(const shader_program_props& props);
    void shader_program_use(u32 shader_program);
    void shader_program_free(u32 shader_program);

}