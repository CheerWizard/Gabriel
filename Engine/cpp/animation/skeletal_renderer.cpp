#include <animation/skeletal_renderer.h>

namespace gl {

    void SkeletalRenderer::use() {
        mShader.use();
    }

    void SkeletalRenderer::updateBones(std::vector<glm::mat4>& bones) {
        UniformArrayM4F uniformBoneTransforms = { "bone_transforms", bones };
        mShader.use();
        mShader.setUniformArray(uniformBoneTransforms);
    }

    SkeletalRenderer::~SkeletalRenderer() {
        mShader.free();
    }

    void SkeletalRenderer::render(Transform& transform, DrawableElements& drawable) {
        transform.update(mShader);
        drawable.draw();
    }

    void SkeletalRenderer::render(Transform& transform, DrawableElements& drawable, Material& material) {
        transform.update(mShader);
        material.update(mShader, 0);
        drawable.draw();
    }

}