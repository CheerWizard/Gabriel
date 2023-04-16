#include <device.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace gl {

    int device::max_attrs_allowed;
    float device::max_anisotropy_samples;

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

        Debugger::init();
    }

    void resize(int w, int h) {
        glViewport(0, 0, w, h);
    }

}
