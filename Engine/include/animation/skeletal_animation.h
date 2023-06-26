#pragma once

#include "io/skeletal_loader.h"

namespace gl {

    struct GABRIEL_API BoneAnimator final {
        Bone* bone = null;

        void update(float dt);
        glm::mat4 interpolatePosition(float dt);
        glm::mat4 interpolateRotation(float dt);
        glm::mat4 interpolateScale(float dt);
    };

    struct GABRIEL_API Animator final {
        float currentTime = 0;
        float dt;
        Animation* animation;
        BoneAnimator boneAnimator;
        std::vector<glm::mat4> boneTransforms;
        bool running = true;

        Animator() = default;

        Animator(Animation* animation);

        void update(float dt);

        void setAnimation(Animation* animation);

        void calculateBoneTransforms(const AssimpNodeData* node, const glm::mat4& parentTransform);

        void play();
        void pause();
        void stop();
    };

}