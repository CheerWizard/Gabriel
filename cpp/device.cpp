#include <device.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace gl {

    int Device::MAX_ATTRS_ALLOWED;
    float Device::MAX_ANISOTROPY_SAMPLES;
    int Device::MAX_WORKGROUP_COUNT[3];
    int Device::MAX_WORKGROUP_SIZE[3];
    int Device::MAX_WORKGROUP_INVOCATIONS;

    void Device::init(int viewport_width, int viewport_height) {
        int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
        if (status == GLFW_FALSE) {
            print_err("Failed to initialize GLAD");
            assert(false);
        }

        Viewport::resize(0, 0, viewport_width, viewport_height);

        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &Device::MAX_ATTRS_ALLOWED);
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &Device::MAX_ANISOTROPY_SAMPLES);

        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &Device::MAX_WORKGROUP_COUNT[0]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &Device::MAX_WORKGROUP_COUNT[1]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &Device::MAX_WORKGROUP_COUNT[2]);

        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &Device::MAX_WORKGROUP_SIZE[0]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &Device::MAX_WORKGROUP_SIZE[1]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &Device::MAX_WORKGROUP_SIZE[2]);

        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &Device::MAX_WORKGROUP_INVOCATIONS);

        Debugger::init();
    }

    void resize(int w, int h) {
        glViewport(0, 0, w, h);
    }

}
