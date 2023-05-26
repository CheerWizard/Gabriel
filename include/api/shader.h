#pragma once

#include <api/uniform.h>
#include <api/image.h>
#include <api/memory_barrier.h>

namespace gl {

    struct ShaderReader final {
        static std::string read(const std::string& path, std::string includeIdentifier = "#include");
    private:
        static void getFilepath(const std::string& fullPath, std::string& pathWithoutFilename);
    };

    struct ShaderStage final {
        u32 id;

        ShaderStage(u32 type, const char* filepath);
    };

    struct Shader {
        u32 id;

        void addVertexStage(const char* filepath);
        void addFragmentStage(const char* filepath);
        void addGeometryStage(const char* filepath);
        void addTessControlStage(const char* filepath);
        void addTessEvalStage(const char* filepath);

        void complete();

        void use();
        static void stop();

        void free();

        void bindSampler(const char* name, int slot, const ImageBuffer& buffer);
        void bindSampler(const ImageSampler& sampler, const ImageBuffer& buffer);

        void bindSamplerStruct(const char* struct_name, const char* name, int slot, const ImageBuffer& buffer);
        void bindSamplerStruct(const char* struct_name, const ImageSampler& sampler, const ImageBuffer& buffer);

        int getUniformLocation(const char* name) const;
        int getUniformArrayLocation(u32 index, const char* name);
        int getUniformStructLocation(const char *structName, const char *fieldName);
        int getUniformArrayStructLocation(const char *structName, const char *fieldName, u32 index);
        int getUniformStructArrayLocation(const char* structName, const char* fieldName, u32 index);
        int getUniformArrayStructArrayLocation(const char* structName, const char* fieldName, u32 struct_index, u32 field_index);

        void setUniform(int location, float value);
        void setUniform(int location, bool value);
        void setUniform(int location, int value);
        void setUniform(int location, double value);
        void setUniform(int location, u32 value);

        void setUniform(int location, glm::fvec2& value);
        void setUniform(int location, glm::fvec3& value);
        void setUniform(int location, glm::fvec4& value);

        void setUniform(int location, glm::ivec2& value);
        void setUniform(int location, glm::ivec3& value);
        void setUniform(int location, glm::ivec4& value);

        void setUniform(int location, glm::bvec2& value);
        void setUniform(int location, glm::bvec3& value);
        void setUniform(int location, glm::bvec4& value);

        void setUniform(int location, glm::dvec3& value);
        void setUniform(int location, glm::dvec2& value);
        void setUniform(int location, glm::dvec4& value);

        void setUniform(int location, glm::fmat2& value);
        void setUniform(int location, glm::fmat3& value);
        void setUniform(int location, glm::fmat4& value);

        void setUniform(int location, glm::dmat2& value);
        void setUniform(int location, glm::dmat3& value);
        void setUniform(int location, glm::dmat4& value);

        template<typename T>
        void setUniform(Uniform<T>& uniform);

        template<typename T>
        void setUniformArgs(const char* name, T& value);

        template<typename T>
        void setUniformArray(UniformArray<T>& uniform_array);

        template<typename T>
        void setUniformStruct(const char* struct_name, Uniform<T>& field_uniform);

        template<typename T>
        void setUniform(const char *name, T &value, int i);

        template<typename T>
        void setUniformStruct(const char* struct_name, UniformArray<T>& uniform_array);

        template<typename T>
        void setUniformStruct(const char* struct_name, Uniform<T>& field_uniform, u32 i);

        template<typename T>
        void setUniformStruct(const char *struct_name, const char *field_name, T &field_value, u32 i);

        template<typename T>
        void setUniformStructArgs(const char *struct_name, const char *field_name, T &field_value);

    protected:
        void addStage(u32 type, const char* filepath);

    protected:
        std::vector<u32> stages;
    };

    template<typename T>
    void Shader::setUniform(Uniform<T> &uniform) {
        setUniform(getUniformLocation(uniform.name), uniform.value);
    }

    template<typename T>
    void Shader::setUniformArray(UniformArray<T> &uniform_array) {
        size_t size = uniform_array.values.size();
        for (u32 i = 0; i < size ; i++) {
            setUniform(
                    getUniformArrayLocation(i, uniform_array.name),
                    uniform_array.values[i]
            );
        }
    }

    template<typename T>
    void Shader::setUniform(const char *name, T &value, int i) {
        setUniform(
                getUniformArrayLocation(i, name),
                value
        );
    }

    template<typename T>
    void Shader::setUniformStruct(const char *struct_name, Uniform<T> &field_uniform) {
        setUniform(getUniformStructLocation(struct_name, field_uniform.name), field_uniform.value);
    }

    template<typename T>
    void Shader::setUniformStruct(const char *struct_name, UniformArray<T> &uniform_array) {
        size_t size = uniform_array.values.size();
        for (u32 i = 0; i < size ; i++) {
            setUniform(
                    getUniformArrayStructLocation(struct_name, uniform_array.name, i),
                    uniform_array.values[i]
            );
        }
    }

    template<typename T>
    void Shader::setUniformStruct(const char *struct_name, Uniform<T> &field_uniform, u32 i) {
        setUniform(
                getUniformStructArrayLocation(struct_name, field_uniform.name, i),
                field_uniform.value
        );
    }

    template<typename T>
    void Shader::setUniformArgs(const char* name, T &value) {
        setUniform(getUniformLocation(name), value);
    }

    template<typename T>
    void Shader::setUniformStructArgs(const char *struct_name, const char *field_name, T &field_value) {
        setUniform(getUniformStructLocation(struct_name, field_name), field_value);
    }

    template<typename T>
    void Shader::setUniformStruct(const char *struct_name, const char *field_name, T &field_value, u32 i) {
        setUniform(getUniformStructArrayLocation(struct_name, field_name, i), field_value);
    }

    struct ComputeShader : Shader {

        void init(const char* filepath);
        void dispatch(u32 x = 1, u32 y = 1, u32 z = 1);

    };

}