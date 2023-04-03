#include <device.h>
#include <primitives.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace gl {

    int device::max_attrs_allowed;
    float device::max_anisotropy_samples;

    void APIENTRY debug_output(
            GLenum source,
            GLenum type,
            u32 id,
            GLenum severity,
            GLsizei length,
            const char* message,
            const void* user_data
    ) {
        // ignore non-significant error/warning codes
        if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

        print("----------------------");
        print("Debug message (" << id << "): " << message);

        switch (source)
        {
            case GL_DEBUG_SOURCE_API:             print("Source: API"); break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   print("Source: Window System"); break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER: print("Source: Shader Compiler"); break;
            case GL_DEBUG_SOURCE_THIRD_PARTY:     print("Source: Third Party"); break;
            case GL_DEBUG_SOURCE_APPLICATION:     print("Source: Application"); break;
            case GL_DEBUG_SOURCE_OTHER:           print("Source: Other"); break;
        }

        switch (type)
        {
            case GL_DEBUG_TYPE_ERROR:               print("Type: Error"); break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: print("Type: Deprecated Behaviour"); break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  print("Type: Undefined Behaviour"); break;
            case GL_DEBUG_TYPE_PORTABILITY:         print("Type: Portability"); break;
            case GL_DEBUG_TYPE_PERFORMANCE:         print("Type: Performance"); break;
            case GL_DEBUG_TYPE_MARKER:              print("Type: Marker"); break;
            case GL_DEBUG_TYPE_PUSH_GROUP:          print("Type: Push Group"); break;
            case GL_DEBUG_TYPE_POP_GROUP:           print("Type: Pop Group"); break;
            case GL_DEBUG_TYPE_OTHER:               print("Type: Other"); break;
        }

        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH:         print("Severity: high"); break;
            case GL_DEBUG_SEVERITY_MEDIUM:       print("Severity: medium"); break;
            case GL_DEBUG_SEVERITY_LOW:          print("Severity: low"); break;
            case GL_DEBUG_SEVERITY_NOTIFICATION: print("Severity: notification"); break;
        }

        if (type == GL_DEBUG_TYPE_ERROR) {
            __debugbreak();
        }
    }

    void init(int viewport_width, int viewport_height) {
        int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
        if (status == GLFW_FALSE) {
            print_err("Failed to initialize GLAD");
            assert(false);
        }

        glViewport(0, 0, viewport_width, viewport_height);

        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &device::max_attrs_allowed);
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &device::max_anisotropy_samples);

        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

#ifdef DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(debug_output, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }
#endif
    }

    void resize(int w, int h) {
        glViewport(0, 0, w, h);
    }

    u32 check_err(const char* file, int line) {
        GLenum errorCode;
        while ((errorCode = glGetError()) != GL_NO_ERROR)
        {
            std::string error;
            switch (errorCode)
            {
                case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
                case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
                case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
                case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
                case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
                case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
                case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
            }
            std::cout << error << " | " << file << " (" << line << ")" << std::endl;
        }
        return errorCode;
    }

}
