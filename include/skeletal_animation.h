#pragma once

#include <skeletal_loader.h>

#include <map>

namespace gl {

    using namespace io;

    struct BoneAnimator final {
        Bone* bone = null;

        void update(float dt);
        glm::mat4 interpolate_position(float dt);
        glm::mat4 interpolate_rotation(float dt);
        glm::mat4 interpolate_scale(float dt);
    };

    struct Animator final {
        float current_time = 0;
        float dt;
        Animation* animation;
        BoneAnimator bone_animator;
        std::vector<glm::mat4> bone_transforms;
        bool running = true;

        Animator() = default;

        Animator(Animation* animation);

        void update(float dt);

        void set_animation(Animation* animation);

        void calculate_bone_transforms(const AssimpNodeData* node, glm::mat4 parent_transform);

        void play();
        void pause();
        void stop();
    };

}