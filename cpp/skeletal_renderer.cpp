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

    void SkeletalRenderer::render(ecs::EntityID entity_id, Transform& transform, DrawableElements& drawable) {
        shader.set_uniform_args("object_id", entity_id);
        transform.update(shader);
        drawable.draw();
    }

    void SkeletalRenderer::render(ecs::EntityID entity_id, Transform& transform, DrawableElements& drawable, Material& material) {
        shader.set_uniform_args("object_id", entity_id);
        transform.update(shader);
        material.update(shader, 0);
        drawable.draw();
    }

}