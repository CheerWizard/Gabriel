#include <shader.h>
#include <file_utils.h>

#include <glad/glad.h>

#include <unordered_map>
#include <sstream>

namespace gl {

    std::string ShaderReader::read(const std::string& path, std::string include_identifier) {
        include_identifier += ' ';
        static bool recursive = false;

        std::string full_src = "";
        std::ifstream file(path);

        if (!file.is_open()) {
            print_err("Failed to open file " << path);
            return full_src;
        }

        std::string line_buffer;
        while (std::getline(file, line_buffer)) {
            // Look for the new shader include identifier
            if (line_buffer.find(include_identifier) != line_buffer.npos) {
                // Remove the include identifier, this will cause the path to remain
                line_buffer.erase(0, include_identifier.size());

                // The include path is relative to the current shader file path
                std::string pathOfThisFile;
                get_filepath(path, pathOfThisFile);
                line_buffer.insert(0, pathOfThisFile);

                // By using recursion, the new include file can be extracted
                // and inserted at this location in the shader source code
                recursive = true;
                full_src += read(line_buffer, include_identifier);

                // Do not add this line to the shader source code, as the include
                // path would generate a compilation issue in the final source code
                continue;
            }

            full_src += line_buffer + '\n';
        }

        // Only add the null terminator at the end of the complete file,
        // essentially skipping recursive function calls this way
        if (!recursive)
            full_src += '\0';

        file.close();

        return full_src;
    }

    void ShaderReader::get_filepath(
            const std::string &full_path,
            std::string &path_without_filename
    ) {
        // Remove the file name and store the path to this folder
        size_t found = full_path.find_last_of("/\\");
        path_without_filename = full_path.substr(0, found + 1);
    }

    ShaderStage::ShaderStage(u32 type, const char *filepath) {
        std::string src = ShaderReader::read(filepath);
        const char* c_src = src.c_str();
        if (src.empty()) {
            print_err("shader_init() : failed to read stage from file");
            print_err(filepath);
            id = 0;
        }

        id = glCreateShader(type);
        glShaderSource(id, 1, &c_src, null);
        glCompileShader(id);

        int status;
        char info[512];
        glGetShaderiv(id, GL_COMPILE_STATUS, &status);
        if (!status) {
            glGetShaderInfoLog(id, 512, null, info);
            print_err("shader_init() : failed stage compilation");
            print_err(info);
            id = 0;
        }
    }

    void Shader::complete() {
        if (stages.empty()) {
            print_err("Shader has no stages to complete");
            return;
        }

        id = glCreateProgram();

        for (u32 stage : stages) {
            glAttachShader(id, stage);
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

        for (u32 stage : stages) {
            glDeleteShader(stage);
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

    void Shader::bind_sampler(const char* name, int slot, const ImageBuffer &buffer) {
        ImageBuffer::bind_activate(buffer.type, buffer.id, slot);
        set_uniform(get_uniform_location(name), slot);
    }

    void Shader::bind_sampler(const ImageSampler& sampler, const ImageBuffer &buffer) {
        bind_sampler(sampler.name, sampler.slot, buffer);
    }

    void Shader::bind_sampler_struct(const char* struct_name, const char* name, int slot, const ImageBuffer &buffer) {
        ImageBuffer::bind_activate(buffer.type, buffer.id, slot);
        set_uniform(get_uniform_struct_location(struct_name, name), slot);
    }

    void Shader::bind_sampler_struct(const char* struct_name, const ImageSampler& sampler, const ImageBuffer &buffer) {
        bind_sampler_struct(struct_name, sampler.name, sampler.slot, buffer);
    }

    void Shader::add_stage(u32 type, const char* filepath) {
        u32 stage = ShaderStage(type, filepath).id;
        if (stage != 0) {
            stages.emplace_back(stage);
        }
    }

    void Shader::add_vertex_stage(const char *filepath) {
        add_stage(GL_VERTEX_SHADER, filepath);
    }

    void Shader::add_fragment_stage(const char *filepath) {
        add_stage(GL_FRAGMENT_SHADER, filepath);
    }

    void Shader::add_geometry_stage(const char *filepath) {
        add_stage(GL_GEOMETRY_SHADER, filepath);
    }

    void Shader::add_tess_control_stage(const char *filepath) {
        add_stage(GL_TESS_CONTROL_SHADER, filepath);
    }

    void Shader::add_tess_eval_stage(const char *filepath) {
        add_stage(GL_TESS_EVALUATION_SHADER, filepath);
    }

    void ComputeShader::init(const char *filepath) {
        add_stage(GL_COMPUTE_SHADER, filepath);
        complete();
    }

    void ComputeShader::dispatch(u32 x, u32 y, u32 z) {
        glDispatchCompute(x, y, z);
    }

}