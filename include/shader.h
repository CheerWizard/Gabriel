#pragma once

#include <primitives.h>
#include <uniform.h>

namespace gl {

    struct shader_props {
        const char* vertex_filepath;
        const char* fragment_filepath;
        const char* geometry_filepath = null; // optional
    };

    u32 shader_init(const shader_props& props);
    void shader_use(u32 shader_program);
    void shader_free(u32 shader_program);

    int get_uniform_location(u32 shader_program, const char* name);
    int get_uniform_array_location(u32 shader_program, u32 index, const char* name);
    int get_uniform_struct_location(u32 shader, const char *structName, const char *fieldName);
    int get_uniform_array_struct_location(u32 shader, const char *structName, const char *fieldName, u32 index);
    int get_uniform_struct_array_location(u32 shader, const char* structName, const char* fieldName, u32 index);
    int get_uniform_array_struct_array_location(u32 shader, const char* structName, const char* fieldName, u32 struct_index, u32 field_index);

    void shader_set_uniform(int location, float value);
    void shader_set_uniform(int location, bool value);
    void shader_set_uniform(int location, int value);
    void shader_set_uniform(int location, double value);

    void shader_set_uniform(int location, glm::fvec2& value);
    void shader_set_uniform(int location, glm::fvec3& value);
    void shader_set_uniform(int location, glm::fvec4& value);

    void shader_set_uniform(int location, glm::ivec2& value);
    void shader_set_uniform(int location, glm::ivec3& value);
    void shader_set_uniform(int location, glm::ivec4& value);

    void shader_set_uniform(int location, glm::bvec2& value);
    void shader_set_uniform(int location, glm::bvec3& value);
    void shader_set_uniform(int location, glm::bvec4& value);

    void shader_set_uniform(int location, glm::dvec3& value);
    void shader_set_uniform(int location, glm::dvec2& value);
    void shader_set_uniform(int location, glm::dvec4& value);

    void shader_set_uniform(int location, glm::fmat2& value);
    void shader_set_uniform(int location, glm::fmat3& value);
    void shader_set_uniform(int location, glm::fmat4& value);

    void shader_set_uniform(int location, glm::dmat2& value);
    void shader_set_uniform(int location, glm::dmat3& value);
    void shader_set_uniform(int location, glm::dmat4& value);

    template<typename T>
    void shader_set_uniform(u32 shader, uniform<T>& uniform) {
        shader_set_uniform(get_uniform_location(shader, uniform.name), uniform.value);
    }

    template<typename T>
    void shader_set_uniform_args(u32 shader, const char* name, T& value) {
        shader_set_uniform(get_uniform_location(shader, name), value);
    }

    constexpr auto shader_set_uniformf = &shader_set_uniform_args<float>;
    constexpr auto shader_set_uniformi = &shader_set_uniform_args<int>;
    constexpr auto shader_set_uniformd = &shader_set_uniform_args<double>;
    constexpr auto shader_set_uniformb = &shader_set_uniform_args<bool>;

    constexpr auto shader_set_uniform2v = &shader_set_uniform_args<glm::vec2>;
    constexpr auto shader_set_uniform3v = &shader_set_uniform_args<glm::vec3>;
    constexpr auto shader_set_uniform4v = &shader_set_uniform_args<glm::vec4>;

    constexpr auto shader_set_uniform2m = &shader_set_uniform_args<glm::mat2>;
    constexpr auto shader_set_uniform3m = &shader_set_uniform_args<glm::mat3>;
    constexpr auto shader_set_uniform4m = &shader_set_uniform_args<glm::mat4>;

    template<typename T>
    void shader_set_uniform(u32 shader, uniform_array<T>& uniform_array) {
        size_t size = uniform_array.values.size();
        for (u32 i = 0; i < size ; i++) {
            shader_set_uniform(
                    get_uniform_array_location(shader, i, uniform_array.name),
                    uniform_array.values[i]
            );
        }
    }

    template<typename T>
    void shader_set_uniform(u32 shader, const char* name, T& value, int i) {
        shader_set_uniform(
                get_uniform_array_location(shader, i, name),
                value
        );
    }

    template<typename T>
    void shader_set_uniform_struct(u32 shader, const char* struct_name, uniform<T>& field_uniform) {
        shader_set_uniform(get_uniform_struct_location(shader, struct_name, field_uniform.name), field_uniform.value);
    }

    template<typename T>
    void shader_set_uniform_struct(u32 shader, const char* struct_name, uniform_array<T>& uniform_array) {
        size_t size = uniform_array.values.size();
        for (u32 i = 0; i < size ; i++) {
            shader_set_uniform(
                    get_uniform_array_struct_location(shader, struct_name, uniform_array.name, i),
                    uniform_array.values[i]
            );
        }
    }

    template<typename T>
    void shader_set_uniform_struct(u32 shader, const char* struct_name, uniform<T>& field_uniform, u32 i) {
        shader_set_uniform(
                get_uniform_struct_array_location(shader, struct_name, field_uniform.name, i),
                field_uniform.value
        );
    }

    template<typename T>
    void shader_set_uniform_struct_args(u32 shader, const char* struct_name, const char* field_name, T& field_value) {
        shader_set_uniform(get_uniform_struct_location(shader, struct_name, field_name), field_value);
    }

    template<typename T>
    void shader_set_uniform_struct(u32 shader, const char* struct_name, const char* field_name, T& field_value, u32 i) {
        shader_set_uniform(get_uniform_struct_array_location(shader, struct_name, field_name, i), field_value);
    }

    constexpr auto shader_set_uniform_structf = &shader_set_uniform_struct_args<float>;
    constexpr auto shader_set_uniform_structi = &shader_set_uniform_struct_args<int>;
    constexpr auto shader_set_uniform_structd = &shader_set_uniform_struct_args<double>;
    constexpr auto shader_set_uniform_structb = &shader_set_uniform_struct_args<bool>;

    constexpr auto shader_set_uniform_struct2v = &shader_set_uniform_struct_args<glm::vec2>;
    constexpr auto shader_set_uniform_struct3v = &shader_set_uniform_struct_args<glm::vec3>;
    constexpr auto shader_set_uniform_struct4v = &shader_set_uniform_struct_args<glm::vec4>;

    constexpr auto shader_set_uniform_struct2m = &shader_set_uniform_struct_args<glm::mat2>;
    constexpr auto shader_set_uniform_struct3m = &shader_set_uniform_struct_args<glm::mat3>;
    constexpr auto shader_set_uniform_struct4m = &shader_set_uniform_struct_args<glm::mat4>;
}