#include <animation/skeletal_renderer.h>

namespace gl {

    void SkeletalRenderer::begin() {
        mShader.use();
    }

    void SkeletalRenderer::updateBones(std::vector<glm::mat4>& bones) {
        UniformArrayM4F uniformBoneTransforms = {"bone_transforms", bones };
        mShader.use();
        mShader.setUniformArray(uniformBoneTransforms);
    }

    SkeletalRenderer::~SkeletalRenderer() {
        mShader.free();
    }

    void SkeletalRenderer::setCameraPos(glm::vec3& cameraPos) {
        mShader.use();
        mShader.setUniformArgs("camera_pos", cameraPos);
    }

    void SkeletalRenderer::render(EntityID entityId, Transform& transform, DrawableElements& drawable) {
        mShader.setUniformArgs("entity_id", entityId);
        transform.update(mShader);
        drawable.draw();
    }

    void SkeletalRenderer::render(EntityID entityId, Transform& transform, DrawableElements& drawable, Material& material) {
        mShader.setUniformArgs("entity_id", entityId);
        transform.update(mShader);
        material.update(mShader, 0);
        drawable.draw();
    }

}