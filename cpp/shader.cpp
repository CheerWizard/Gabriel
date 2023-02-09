#include <shader.h>
#include <file_utils.h>

#include <glad/glad.h>

namespace gl {

    u32 shader_init(const char* filepath, u32 shader_type) {
        std::string src = io::file_read(filepath);
        const char* c_src = src.c_str();
        if (src.empty()) {
            print_err("shader_init() : failed to read shader from file");
            print_err(filepath);
            return 0;
        }

        u32 shader = glCreateShader(shader_type);
        glShaderSource(shader, 1, &c_src, null);
        glCompileShader(shader);

        int status;
        char info[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (!status) {
            glGetShaderInfoLog(shader, 512, null, info);
            print_err("shader_init() : failed shader compilation");
            print_err(info);
            return 0;
        }

        return shader;
    }

    u32 shader_program_init(const shader_program_props& props) {
        u32 vertex_shader = shader_init(props.vertex_filepath, GL_VERTEX_SHADER);
        if (!vertex_shader)
            return 0;

        u32 fragment_shader = shader_init(props.fragment_filepath, GL_FRAGMENT_SHADER);
        if (!fragment_shader)
            return 0;

        u32 shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);
        glLinkProgram(shader_program);

        int status;
        char info[512];
        glGetProgramiv(shader_program, GL_LINK_STATUS, &status);
        if (!status) {
            glGetProgramInfoLog(shader_program, 512, null, info);
            print_err("shader_program_init() : failed shader program linkage");
            print_err(info);
            return 0;
        }

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        return shader_program;
    }

    void shader_program_use(u32 shader_program) {
        glUseProgram(shader_program);
    }

    void shader_program_free(u32 shader_program) {
        glDeleteProgram(shader_program);
    }

}