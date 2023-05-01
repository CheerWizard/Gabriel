#pragma once

#include <uniform.h>
#include <image.h>

namespace gl {

    struct ShaderReader final {
        static std::string read(const std::string& path, std::string include_identifier = "#include");
    private:
        static void get_filepath(const std::string& full_path, std::string& path_without_filename);
    };

    struct ShaderStage final {
        u32 id;

        ShaderStage(u32 type, const char* filepath);
    };

    struct Shader {
        u32 id;

        void add_vertex_stage(const char* filepath);
        void add_fragment_stage(const char* filepath);
        void add_geometry_stage(const char* filepath);
        void add_tess_control_stage(const char* filepath);
        void add_tess_eval_stage(const char* filepath);

        void complete();

        void use();
        static void stop();

        void free();

        void bind_sampler(const char* name, int slot, const ImageBuffer& buffer);
        void bind_sampler(const ImageSampler& sampler, const ImageBuffer& buffer);

        void bind_sampler_struct(const char* struct_name, const char* name, int slot, const ImageBuffer& buffer);
        void bind_sampler_struct(const char* struct_name, const ImageSampler& sampler, const ImageBuffer& buffer);

        int get_uniform_location(const char* name) const;
        int get_uniform_array_location(u32 index, const char* name);
        int get_uniform_struct_location(const char *structName, const char *fieldName);
        int get_uniform_array_struct_location(const char *structName, const char *fieldName, u32 index);
        int get_uniform_struct_array_location(const char* structName, const char* fieldName, u32 index);
        int get_uniform_array_struct_array_location(const char* structName, const char* fieldName, u32 struct_index, u32 field_index);

        void set_uniform(int location, float value);
        void set_uniform(int location, bool value);
        void set_uniform(int location, int value);
        void set_uniform(int location, double value);
        void set_uniform(int location, u32 value);

        void set_uniform(int location, glm::fvec2& value);
        void set_uniform(int location, glm::fvec3& value);
        void set_uniform(int location, glm::fvec4& value);

        void set_uniform(int location, glm::ivec2& value);
        void set_uniform(int location, glm::ivec3& value);
        void set_uniform(int location, glm::ivec4& value);

        void set_uniform(int location, glm::bvec2& value);
        void set_uniform(int location, glm::bvec3& value);
        void set_uniform(int location, glm::bvec4& value);

        void set_uniform(int location, glm::dvec3& value);
        void set_uniform(int location, glm::dvec2& value);
        void set_uniform(int location, glm::dvec4& value);

        void set_uniform(int location, glm::fmat2& value);
        void set_uniform(int location, glm::fmat3& value);
        void set_uniform(int location, glm::fmat4& value);

        void set_uniform(int location, glm::dmat2& value);
        void set_uniform(int location, glm::dmat3& value);
        void set_uniform(int location, glm::dmat4& value);

        template<typename T>
        void set_uniform(Uniform<T>& uniform);

        template<typename T>
        void set_uniform_args(const char* name, T& value);

        template<typename T>
        void set_uniform_array(UniformArray<T>& uniform_array);

        template<typename T>
        void set_uniform_struct(const char* struct_name, Uniform<T>& field_uniform);

        template<typename T>
        void set_uniform(const char *name, T &value, int i);

        template<typename T>
        void set_uniform_struct(const char* struct_name, UniformArray<T>& uniform_array);

        template<typename T>
        void set_uniform_struct(const char* struct_name, Uniform<T>& field_uniform, u32 i);

        template<typename T>
        void set_uniform_struct(const char *struct_name, const char *field_name, T &field_value, u32 i);

        template<typename T>
        void set_uniform_struct_args(const char *struct_name, const char *field_name, T &field_value);

    protected:
        void add_stage(u32 type, const char* filepath);

    protected:
        std::vector<u32> stages;
    };

    template<typename T>
    void Shader::set_uniform(Uniform<T> &uniform) {
        set_uniform(get_uniform_location(uniform.name), uniform.value);
    }

    template<typename T>
    void Shader::set_uniform_array(UniformArray<T> &uniform_array) {
        size_t size = uniform_array.values.size();
        for (u32 i = 0; i < size ; i++) {
            set_uniform(
                    get_uniform_array_location(i, uniform_array.name),
                    uniform_array.values[i]
            );
        }
    }

    template<typename T>
    void Shader::set_uniform(const char *name, T &value, int i) {
        set_uniform(
                get_uniform_array_location(i, name),
                value
        );
    }

    template<typename T>
    void Shader::set_uniform_struct(const char *struct_name, Uniform<T> &field_uniform) {
        set_uniform(get_uniform_struct_location(struct_name, field_uniform.name), field_uniform.value);
    }

    template<typename T>
    void Shader::set_uniform_struct(const char *struct_name, UniformArray<T> &uniform_array) {
        size_t size = uniform_array.values.size();
        for (u32 i = 0; i < size ; i++) {
            set_uniform(
                    get_uniform_array_struct_location(struct_name, uniform_array.name, i),
                    uniform_array.values[i]
            );
        }
    }

    template<typename T>
    void Shader::set_uniform_struct(const char *struct_name, Uniform<T> &field_uniform, u32 i) {
        set_uniform(
                get_uniform_struct_array_location(struct_name, field_uniform.name, i),
                field_uniform.value
        );
    }

    template<typename T>
    void Shader::set_uniform_args(const char *name, T &value) {
        set_uniform(get_uniform_location(name), value);
    }

    template<typename T>
    void Shader::set_uniform_struct_args(const char *struct_name, const char *field_name, T &field_value) {
        set_uniform(get_uniform_struct_location(struct_name, field_name), field_value);
    }

    template<typename T>
    void Shader::set_uniform_struct(const char *struct_name, const char *field_name, T &field_value, u32 i) {
        set_uniform(get_uniform_struct_array_location(struct_name, field_name, i), field_value);
    }

    struct ComputeShader : Shader {

        void init(const char* filepath);
        void dispatch(u32 x = 1, u32 y = 1, u32 z = 1);

    };

}