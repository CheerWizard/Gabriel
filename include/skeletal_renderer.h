#pragma once

#include <shader.h>
#include <transform.h>
#include <material.h>
#include <draw.h>

namespace gl {

    struct SkeletalRenderer {

        void free();

        void begin();

        void set_camera_pos(glm::vec3& camera_pos);
        void update_bones(std::vector<glm::mat4>& bones);

        void render(u32 object_id, DrawableElements* drawable, Transform* transform);
        void render(u32 object_id, DrawableElements* drawable, Transform* transform, Material* material);

    protected:
        Shader shader;
    };

}