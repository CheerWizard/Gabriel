#include <shader.h>
#include <file_utils.h>

#include <glad/glad.h>

#include <unordered_map>
#include <sstream>

namespace gl {

    static u32 shader_init(const char* filepath, u32 shader_type) {
        std::string src = io::read_file_string(filepath);
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

    void Shader::init(const char *vertex_filepath, const char *fragment_filepath, const char *geometry_filepath) {
        u32 vertex_shader = shader_init(vertex_filepath, GL_VERTEX_SHADER);
        if (!vertex_shader)
            return;

        u32 fragment_shader = shader_init(fragment_filepath, GL_FRAGMENT_SHADER);
        if (!fragment_shader)
            return;

        std::vector<u32> shaders = { vertex_shader, fragment_shader };

        if (geometry_filepath) {
            u32 geometry_shader = shader_init(geometry_filepath, GL_GEOMETRY_SHADER);
            if (geometry_shader) {
                shaders.emplace_back(geometry_shader);
            }
        }

        id = glCreateProgram();
        for (u32 shader : shaders) {
            glAttachShader(id, shader);
        }
        glLinkProgram(id);

        int status;
        char info[512];
        glGetProgramiv(id, GL_LINK_STATUS, &status);
        if (!status) {
            glGetProgramInfoLog(id, 512, null, info);
            print_err("shader_program_init() : failed shader program linkage");
            print_err(info);
            return;
        }

        for (u32 shader : shaders) {
            glDeleteShader(shader);
        }
    }

    void Shader::use() {
        glUseProgram(id);
    }

    void Shader::stop() {
        glUseProgram(0);
    }

    void Shader::free() {
        glDeleteProgram(id);
    }

    static std::unordered_map<const char*, int> s_uniform_locations;

    int Shader::get_uniform_location(const char* name) const {
        // todo found hash key collisions when updating light struct field
//        if (s_uniform_locations.find(name) == s_uniform_locations.end()) {
            int location = glGetUniformLocation(id, name);
//            s_uniform_locations.insert({ name, location });
            return location;
//        } else {
//            return s_uniform_locations[name];
//        }
    }

    int Shader::get_uniform_array_location(u32 index, const char* name) {
        std::stringstream ss;
        ss << name << "[" << index << "]";
        std::string s = ss.str();
        return get_uniform_location(s.c_str());
    }

    int Shader::get_uniform_struct_location(const char *structName, const char *fieldName) {
        std::stringstream ss;
        ss << structName << "." << fieldName;
        std::string s = ss.str();
        return get_uniform_location(s.c_str());
    }

    int Shader::get_uniform_array_struct_location(const char *structName, const char *fieldName, u32 index) {
        std::stringstream ss;
        ss << structName << "." << fieldName << "[" << index << "]";
        std::string s = ss.str();
        return get_uniform_location(s.c_str());
    }

    int Shader::get_uniform_struct_array_location(const char* structName, const char* fieldName, u32 index) {
        std::stringstream ss;
        ss << structName << "[" << index << "]" << "." << fieldName;
        std::string s = ss.str();
        return get_uniform_location(s.c_str());
    }

    int Shader::get_uniform_array_struct_array_location(const char* structName, const char* fieldName, u32 struct_index, u32 field_index) {
        std::stringstream ss;
        ss << structName << "[" << struct_index << "]" << "." << fieldName << "[" << field_index << "]";
        std::string s = ss.str();
        return get_uniform_location(s.c_str());
    }

    void Shader::set_uniform(int location, float value) {
        glUniform1f(location, value);
    }

    void Shader::set_uniform(int location, bool value) {
        glUniform1i(location, value);
    }

    void Shader::set_uniform(int location, int value) {
        glUniform1i(location, value);
    }

    void Shader::set_uniform(int location, double value) {
        glUniform1d(location, value);
    }

    void Shader::set_uniform(int location, u32 value) {
        glUniform1ui(location, value);
    }

    void Shader::set_uniform(int location, glm::fvec2& value) {
        glUniform2fv(location, 1, glm::value_ptr(value));
    }

    void Shader::set_uniform(int location, glm::fvec3& value) {
        glUniform3fv(location, 1, glm::value_ptr(value));
    }

    void Shader::set_uniform(int location, glm::fvec4& value) {
        glUniform4fv(location, 1, glm::value_ptr(value));
    }

    void Shader::set_uniform(int location, glm::ivec2& value) {
        glUniform2iv(location, 1, glm::value_ptr(value));
    }

    void Shader::set_uniform(int location, glm::ivec3& value) {
        glUniform3iv(location, 1, glm::value_ptr(value));
    }

    void Shader::set_uniform(int location, glm::ivec4& value) {
        glUniform4iv(location, 1, glm::value_ptr(value));
    }

    void Shader::set_uniform(int location, glm::bvec2& value) {
        glUniform2iv(location, 1, (const int*)glm::value_ptr(value));
    }

    void Shader::set_uniform(int location, glm::bvec3& value) {
        glUniform3iv(location, 1, (const int*)glm::value_ptr(value));
    }

    void Shader::set_uniform(int location, glm::bvec4& value) {
        glUniform4iv(location, 1, (const int*)glm::value_ptr(value));
    }

    void Shader::set_uniform(int location, glm::dvec2& value) {
        glUniform2dv(location, 1, glm::value_ptr(value));
    }

    void Shader::set_uniform(int location, glm::dvec3& value) {
        glUniform3dv(location, 1, glm::value_ptr(value));
    }

    void Shader::set_uniform(int location, glm::dvec4& value) {
        glUniform4dv(location, 1, glm::value_ptr(value));
    }

    void Shader::set_uniform(int location, glm::fmat2& value) {
        glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::set_uniform(int location, glm::fmat3& value) {
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::set_uniform(int location, glm::fmat4& value) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::set_uniform(int location, glm::dmat2& value) {
        glUniformMatrix2dv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::set_uniform(int location, glm::dmat3& value) {
        glUniformMatrix3dv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::set_uniform(int location, glm::dmat4& value) {
        glUniformMatrix4dv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::bind_sampler(const char* name, int slot, const Texture &texture) {
        Texture::bind_activate(texture.type, texture.id, slot);
        set_uniform(get_uniform_location(name), slot);
    }

    void Shader::bind_sampler(const TextureSampler& sampler, const Texture &texture) {
        bind_sampler(sampler.name, sampler.slot, texture);
    }

    void Shader::bind_sampler_struct(const char* struct_name, const char* name, int slot, const Texture &texture) {
        Texture::bind_activate(texture.type, texture.id, slot);
        set_uniform(get_uniform_struct_location(struct_name, name), slot);
    }

    void Shader::bind_sampler_struct(const char* struct_name, const TextureSampler& sampler, const Texture &texture) {
        bind_sampler_struct(struct_name, sampler.name, sampler.slot, texture);
    }

}