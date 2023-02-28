#include <shader.h>
#include <file_utils.h>

#include <glad/glad.h>

#include <unordered_map>
#include <sstream>

namespace gl {

    static u32 shader_init(const char* filepath, u32 shader_type) {
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

    u32 shader_init(const shader_props &props) {
        u32 vertex_shader = shader_init(props.vertex_filepath, GL_VERTEX_SHADER);
        if (!vertex_shader)
            return 0;

        u32 fragment_shader = shader_init(props.fragment_filepath, GL_FRAGMENT_SHADER);
        if (!fragment_shader)
            return 0;

        std::vector<u32> shaders = { vertex_shader, fragment_shader };

        if (props.geometry_filepath) {
            u32 geometry_shader = shader_init(props.geometry_filepath, GL_GEOMETRY_SHADER);
            if (geometry_shader) {
                shaders.emplace_back(geometry_shader);
            }
        }

        u32 shader_program = glCreateProgram();
        for (u32 shader : shaders) {
            glAttachShader(shader_program, shader);
        }
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


        for (u32 shader : shaders) {
            glDeleteShader(shader);
        }

        return shader_program;
    }

    void shader_use(u32 shader) {
        glUseProgram(shader);
    }

    void shader_free(u32 shader) {
        glDeleteProgram(shader);
    }

    static std::unordered_map<const char*, int> s_uniform_locations;

    int get_uniform_location(u32 shader, const char* name) {
        // todo found hash key collisions when updating light struct field
//        if (s_uniform_locations.find(name) == s_uniform_locations.end()) {
            int location = glGetUniformLocation(shader, name);
//            s_uniform_locations.insert({ name, location });
            return location;
//        } else {
//            return s_uniform_locations[name];
//        }
    }

    int get_uniform_array_location(u32 shader, u32 index, const char* name) {
        std::stringstream ss;
        ss << name << "[" << index << "]";
        std::string s = ss.str();
        return get_uniform_location(shader, s.c_str());
    }

    int get_uniform_struct_location(u32 shader, const char *structName, const char *fieldName) {
        std::stringstream ss;
        ss << structName << "." << fieldName;
        std::string s = ss.str();
        return get_uniform_location(shader, s.c_str());
    }

    int get_uniform_array_struct_location(u32 shader, const char *structName, const char *fieldName, u32 index) {
        std::stringstream ss;
        ss << structName << "." << fieldName << "[" << index << "]";
        std::string s = ss.str();
        return get_uniform_location(shader, s.c_str());
    }

    int get_uniform_struct_array_location(u32 shader, const char* structName, const char* fieldName, u32 index) {
        std::stringstream ss;
        ss << structName << "[" << index << "]" << "." << fieldName;
        std::string s = ss.str();
        return get_uniform_location(shader, s.c_str());
    }

    int get_uniform_array_struct_array_location(u32 shader, const char* structName, const char* fieldName, u32 struct_index, u32 field_index) {
        std::stringstream ss;
        ss << structName << "[" << struct_index << "]" << "." << fieldName << "[" << field_index << "]";
        std::string s = ss.str();
        return get_uniform_location(shader, s.c_str());
    }

    void shader_set_uniform(int location, float value) {
        glUniform1f(location, value);
    }

    void shader_set_uniform(int location, bool value) {
        glUniform1i(location, value);
    }

    void shader_set_uniform(int location, int value) {
        glUniform1i(location, value);
    }

    void shader_set_uniform(int location, double value) {
        glUniform1d(location, value);
    }

    void shader_set_uniform(int location, glm::fvec2& value) {
        glUniform2fv(location, 1, glm::value_ptr(value));
    }

    void shader_set_uniform(int location, glm::fvec3& value) {
        glUniform3fv(location, 1, glm::value_ptr(value));
    }

    void shader_set_uniform(int location, glm::fvec4& value) {
        glUniform4fv(location, 1, glm::value_ptr(value));
    }

    void shader_set_uniform(int location, glm::ivec2& value) {
        glUniform2iv(location, 1, glm::value_ptr(value));
    }

    void shader_set_uniform(int location, glm::ivec3& value) {
        glUniform3iv(location, 1, glm::value_ptr(value));
    }

    void shader_set_uniform(int location, glm::ivec4& value) {
        glUniform4iv(location, 1, glm::value_ptr(value));
    }

    void shader_set_uniform(int location, glm::bvec2& value) {
        glUniform2iv(location, 1, (const int*)glm::value_ptr(value));
    }

    void shader_set_uniform(int location, glm::bvec3& value) {
        glUniform3iv(location, 1, (const int*)glm::value_ptr(value));
    }

    void shader_set_uniform(int location, glm::bvec4& value) {
        glUniform4iv(location, 1, (const int*)glm::value_ptr(value));
    }

    void shader_set_uniform(int location, glm::dvec2& value) {
        glUniform2dv(location, 1, glm::value_ptr(value));
    }

    void shader_set_uniform(int location, glm::dvec3& value) {
        glUniform3dv(location, 1, glm::value_ptr(value));
    }

    void shader_set_uniform(int location, glm::dvec4& value) {
        glUniform4dv(location, 1, glm::value_ptr(value));
    }

    void shader_set_uniform(int location, glm::fmat2& value) {
        glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void shader_set_uniform(int location, glm::fmat3& value) {
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void shader_set_uniform(int location, glm::fmat4& value) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void shader_set_uniform(int location, glm::dmat2& value) {
        glUniformMatrix2dv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void shader_set_uniform(int location, glm::dmat3& value) {
        glUniformMatrix3dv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void shader_set_uniform(int location, glm::dmat4& value) {
        glUniformMatrix4dv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

}