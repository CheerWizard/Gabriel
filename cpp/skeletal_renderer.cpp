#include <skeletal_renderer.h>

namespace gl {

    void SkeletalRenderer::begin() {
        shader.use();
    }

    void SkeletalRenderer::update_bones(std::vector<glm::mat4>& bone_transforms) {
        UniformArrayM4F uniform_bone_transforms = { "bone_transforms", bone_transforms };
        shader.use();
        shader.set_uniform_array(uniform_bone_transforms);
    }

    void SkeletalRenderer::free() {
        shader.free();
    }

    void SkeletalRenderer::set_camera_pos(glm::vec3& camera_pos) {
        shader.use();
        shader.set_uniform_args("camera_pos", camera_pos);
    }

    void SkeletalRenderer::render(u32 object_id, DrawableElements* drawable, Transform* transform) {
        shader.set_uniform_args("object_id", object_id);
        transform->update(shader);
        drawable->draw();
    }

    void SkeletalRenderer::render(u32 object_id, DrawableElements* drawable, Transform* transform, Material* material) {
        shader.set_uniform_args("object_id", object_id);
        transform->update(shader);
        material->update(shader, 0);
        drawable->draw();
    }

}