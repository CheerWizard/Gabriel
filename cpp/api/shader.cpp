#include <api/shader.h>

#include <io/writers.h>

#include <unordered_map>
#include <sstream>
#include <filesystem>

namespace gl {

    std::string ShaderReader::read(const std::string& path, std::string includeIdentifier) {
        includeIdentifier += ' ';
        static bool recursive = false;

        std::string fullSrc = "";
        std::ifstream file(path);

        if (!file.is_open()) {
            error("Failed to open file {0}", path.c_str());
            return fullSrc;
        }

        std::string lineBuffer;
        while (std::getline(file, lineBuffer)) {
            // Look for the new mShader include identifier
            if (lineBuffer.find(includeIdentifier) != lineBuffer.npos) {
                // Remove the include identifier, this will cause the path to remain
                lineBuffer.erase(0, includeIdentifier.size());

                // The include path is relative to the current mShader file path
                std::string pathOfThisFile;
                getFilepath(path, pathOfThisFile);
                lineBuffer.insert(0, pathOfThisFile);

                // By using recursion, the new include file can be extracted
                // and inserted at this location in the mShader source code
                recursive = true;
                fullSrc += read(lineBuffer, includeIdentifier);

                // Do not add this line to the mShader source code, as the include
                // path would generate a compilation issue in the final source code
                continue;
            }

            fullSrc += lineBuffer + '\n';
        }

        // Only add the null terminator at the end of the complete file,
        // essentially skipping recursive function calls this way
        if (!recursive)
            fullSrc += '\0';

        file.close();

        return fullSrc;
    }

    void ShaderReader::getFilepath(
            const std::string& fullPath,
            std::string& pathWithoutFilename
    ) {
        // Remove the file name and store the path to this folder
        size_t found = fullPath.find_last_of("/\\");
        pathWithoutFilename = fullPath.substr(0, found + 1);
    }

    ShaderStage::ShaderStage(u32 type, const char* filepath) {

        // read and preprocess shader from filepath
        std::string src = ShaderReader::read(filepath);
        const char* cSrc = src.c_str();
        if (src.empty()) {
            error("Failed to read stage from file {0}", filepath);
            id = 0;
        }

#ifdef DEBUG
        // write preprocessed shader into file
        std::string generatedFilepath = "generated/" + std::string(filepath);
        std::filesystem::create_directory("generated/shaders");
        FileWriter::write(generatedFilepath.c_str(), src);
#endif

        // create and compile shader
        id = glCreateShader(type);
        glShaderSource(id, 1, &cSrc, null);
        glCompileShader(id);

        // validate compilation
        int status;
        char info[512];
        glGetShaderiv(id, GL_COMPILE_STATUS, &status);
        if (!status) {
            glGetShaderInfoLog(id, 512, null, info);
            error("Failed stage compilation {0}", info);
            id = 0;
        }
    }

    void Shader::complete() {
        if (stages.empty()) {
            error("Shader has no stages to complete");
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
            error("Failed shader program linkage {0}", info);
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

    int Shader::getUniformLocation(const char* name) const {
        return glGetUniformLocation(id, name);
    }

    int Shader::getUniformArrayLocation(u32 index, const char* name) {
        std::stringstream ss;
        ss << name << "[" << index << "]";
        std::string s = ss.str();
        return getUniformLocation(s.c_str());
    }

    int Shader::getUniformStructLocation(const char *structName, const char *fieldName) {
        std::stringstream ss;
        ss << structName << "." << fieldName;
        std::string s = ss.str();
        return getUniformLocation(s.c_str());
    }

    int Shader::getUniformArrayStructLocation(const char *structName, const char *fieldName, u32 index) {
        std::stringstream ss;
        ss << structName << "." << fieldName << "[" << index << "]";
        std::string s = ss.str();
        return getUniformLocation(s.c_str());
    }

    int Shader::getUniformStructArrayLocation(const char* structName, const char* fieldName, u32 index) {
        std::stringstream ss;
        ss << structName << "[" << index << "]" << "." << fieldName;
        std::string s = ss.str();
        return getUniformLocation(s.c_str());
    }

    int Shader::getUniformArrayStructArrayLocation(const char* structName, const char* fieldName, u32 struct_index, u32 field_index) {
        std::stringstream ss;
        ss << structName << "[" << struct_index << "]" << "." << fieldName << "[" << field_index << "]";
        std::string s = ss.str();
        return getUniformLocation(s.c_str());
    }

    void Shader::setUniform(int location, float value) {
        glUniform1f(location, value);
    }

    void Shader::setUniform(int location, bool value) {
        glUniform1i(location, value);
    }

    void Shader::setUniform(int location, int value) {
        glUniform1i(location, value);
    }

    void Shader::setUniform(int location, double value) {
        glUniform1d(location, value);
    }

    void Shader::setUniform(int location, u32 value) {
        glUniform1ui(location, value);
    }

    void Shader::setUniform(int location, glm::fvec2& value) {
        glUniform2fv(location, 1, glm::value_ptr(value));
    }

    void Shader::setUniform(int location, glm::fvec3& value) {
        glUniform3fv(location, 1, glm::value_ptr(value));
    }

    void Shader::setUniform(int location, glm::fvec4& value) {
        glUniform4fv(location, 1, glm::value_ptr(value));
    }

    void Shader::setUniform(int location, glm::ivec2& value) {
        glUniform2iv(location, 1, glm::value_ptr(value));
    }

    void Shader::setUniform(int location, glm::ivec3& value) {
        glUniform3iv(location, 1, glm::value_ptr(value));
    }

    void Shader::setUniform(int location, glm::ivec4& value) {
        glUniform4iv(location, 1, glm::value_ptr(value));
    }

    void Shader::setUniform(int location, glm::bvec2& value) {
        glUniform2iv(location, 1, (const int*)glm::value_ptr(value));
    }

    void Shader::setUniform(int location, glm::bvec3& value) {
        glUniform3iv(location, 1, (const int*)glm::value_ptr(value));
    }

    void Shader::setUniform(int location, glm::bvec4& value) {
        glUniform4iv(location, 1, (const int*)glm::value_ptr(value));
    }

    void Shader::setUniform(int location, glm::dvec2& value) {
        glUniform2dv(location, 1, glm::value_ptr(value));
    }

    void Shader::setUniform(int location, glm::dvec3& value) {
        glUniform3dv(location, 1, glm::value_ptr(value));
    }

    void Shader::setUniform(int location, glm::dvec4& value) {
        glUniform4dv(location, 1, glm::value_ptr(value));
    }

    void Shader::setUniform(int location, glm::fmat2& value) {
        glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::setUniform(int location, glm::fmat3& value) {
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::setUniform(int location, glm::fmat4& value) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::setUniform(int location, glm::dmat2& value) {
        glUniformMatrix2dv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::setUniform(int location, glm::dmat3& value) {
        glUniformMatrix3dv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::setUniform(int location, glm::dmat4& value) {
        glUniformMatrix4dv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::bindSampler(const char* name, int slot, const ImageBuffer &buffer) {
        ImageBuffer::bindActivate(buffer.type, buffer.id, slot);
        setUniform(getUniformLocation(name), slot);
    }

    void Shader::bindSampler(const ImageSampler& sampler, const ImageBuffer &buffer) {
        bindSampler(sampler.name, sampler.slot, buffer);
    }

    void Shader::bindSamplerStruct(const char* struct_name, const char* name, int slot, const ImageBuffer &buffer) {
        ImageBuffer::bindActivate(buffer.type, buffer.id, slot);
        setUniform(getUniformStructLocation(struct_name, name), slot);
    }

    void Shader::bindSamplerStruct(const char* struct_name, const ImageSampler& sampler, const ImageBuffer &buffer) {
        bindSamplerStruct(struct_name, sampler.name, sampler.slot, buffer);
    }

    void Shader::addStage(u32 type, const char* filepath) {
        u32 stage = ShaderStage(type, filepath).id;
        if (stage != 0) {
            stages.emplace_back(stage);
        }
    }

    void Shader::addVertexStage(const char *filepath) {
        addStage(GL_VERTEX_SHADER, filepath);
    }

    void Shader::addFragmentStage(const char *filepath) {
        addStage(GL_FRAGMENT_SHADER, filepath);
    }

    void Shader::addGeometryStage(const char *filepath) {
        addStage(GL_GEOMETRY_SHADER, filepath);
    }

    void Shader::addTessControlStage(const char *filepath) {
        addStage(GL_TESS_CONTROL_SHADER, filepath);
    }

    void Shader::addTessEvalStage(const char *filepath) {
        addStage(GL_TESS_EVALUATION_SHADER, filepath);
    }

    void ComputeShader::init(const char *filepath) {
        addStage(GL_COMPUTE_SHADER, filepath);
        complete();
    }

    void ComputeShader::dispatch(u32 x, u32 y, u32 z) {
        glDispatchCompute(x, y, z);
    }

}