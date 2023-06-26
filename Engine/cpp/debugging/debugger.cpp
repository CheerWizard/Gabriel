#include <debugging/debugger.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace gl {

    void APIENTRY debugOutput(
            GLenum source,
            GLenum type,
            u32 id,
            GLenum severity,
            GLsizei length,
            const char* message,
            const void* userData
    ) {
        std::stringstream ss;

        ss << "ID: " << id << "\n";

        switch (source)
        {
            case GL_DEBUG_SOURCE_API:             ss << "Source: API"; break;
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   ss << "Source: Window System"; break;
            case GL_DEBUG_SOURCE_SHADER_COMPILER: ss << "Source: Shader Compiler"; break;
            case GL_DEBUG_SOURCE_THIRD_PARTY:     ss << "Source: Third Party"; break;
            case GL_DEBUG_SOURCE_APPLICATION:     ss << "Source: Application"; break;
            case GL_DEBUG_SOURCE_OTHER:           ss << "Source: Other"; break;
        }
        ss << "\n";

        switch (type)
        {
            case GL_DEBUG_TYPE_ERROR:               ss << "Type: Error"; break;
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: ss << "Type: Deprecated Behaviour"; break;
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  ss << "Type: Undefined Behaviour"; break;
            case GL_DEBUG_TYPE_PORTABILITY:         ss << "Type: Portability"; break;
            case GL_DEBUG_TYPE_PERFORMANCE:         ss << "Type: Performance"; break;
            case GL_DEBUG_TYPE_MARKER:              ss << "Type: Marker"; break;
            case GL_DEBUG_TYPE_PUSH_GROUP:          ss << "Type: Push Group"; break;
            case GL_DEBUG_TYPE_POP_GROUP:           ss << "Type: Pop Group"; break;
            case GL_DEBUG_TYPE_OTHER:               ss << "Type: Other"; break;
        }
        ss << "\n";

        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH:         ss << "Severity: high"; break;
            case GL_DEBUG_SEVERITY_MEDIUM:       ss << "Severity: medium"; break;
            case GL_DEBUG_SEVERITY_LOW:          ss << "Severity: low"; break;
            case GL_DEBUG_SEVERITY_NOTIFICATION: ss << "Severity: notification"; break;
        }
        ss << "\n";

        ss << "Message: " << message;

        if (type == GL_DEBUG_TYPE_ERROR) {
            error(ss.str().c_str());
            debug_break();
        } else {
            info(ss.str().c_str());
        }
    }

    Debugger* Debugger::sInstance = null;

    Debugger::Debugger() {
        sInstance = this;
#ifdef DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(debugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }
#endif
    }

    Debugger::~Debugger() = default;

    bool Debugger::checkError() {
#ifdef DEBUG
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
            error_trace(error.c_str());
        }

        return errorCode == GL_NO_ERROR;
#else
        return true;
#endif
    }

}