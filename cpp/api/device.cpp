#include <api/device.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace gl {

    Device* Device::sInstance = null;

    Device::Device(int width, int height) {
        sInstance = this;

        int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
        if (status == GLFW_FALSE) {
            error("Failed to initialize GLAD");
            assert(false);
        }

        Viewport::resize(0, 0, width, height);

        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &MAX_ATTRS_ALLOWED);
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &MAX_ANISOTROPY_SAMPLES);

        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &MAX_WORKGROUP_COUNT[0]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &MAX_WORKGROUP_COUNT[1]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &MAX_WORKGROUP_COUNT[2]);

        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &MAX_WORKGROUP_SIZE[0]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &MAX_WORKGROUP_SIZE[1]);
        glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &MAX_WORKGROUP_SIZE[2]);

        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &MAX_WORKGROUP_INVOCATIONS);

        setPolygonOffset(1, 1);
    }

    Device::~Device() = default;

    void Device::setPolygonOffset(float factor, float units) {
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(factor, units);
    }

}
