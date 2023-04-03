#include <skeletal_animation.h>

namespace gl {

    void BoneAnimator::update(float dt) {
        glm::mat4 translation = interpolate_position(dt);
        glm::mat4 rotation = interpolate_rotation(dt);
        glm::mat4 scale = interpolate_scale(dt);
        bone->transform = translation * rotation * scale;
    }

    static float get_scale_factor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
        return (animationTime - lastTimeStamp) / (nextTimeStamp - lastTimeStamp);
    }

    glm::mat4 BoneAnimator::interpolate_position(float dt) {
        auto& positions = bone->positions;

        if (positions.size() == 1)
            return glm::translate(glm::mat4(1.0f), positions[0].position);

        int p0 = 0;
        size_t position_size = positions.size();
        for (int index = 0; index < position_size - 1; ++index) {
            if (dt < positions[index + 1].timestamp) {
                p0 = index;
                break;
            }
        }
        int p1 = p0 + 1;

        float scale_factor = get_scale_factor(positions[p0].timestamp, positions[p1].timestamp, dt);
        glm::vec3 final_position = glm::mix(positions[p0].position, positions[p1].position, scale_factor);
        return glm::translate(glm::mat4(1.0f), final_position);
    }

    glm::mat4 BoneAnimator::interpolate_rotation(float dt) {
        auto& rotations = bone->rotations;

        if (rotations.size() == 1)
            return glm::mat4(glm::normalize(rotations[0].orientation));

        int p0 = 0;
        size_t rotation_size = rotations.size();
        for (int index = 0; index < rotation_size - 1; ++index) {
            if (dt < rotations[index + 1].timestamp) {
                p0 = index;
                break;
            }
        }
        int p1 = p0 + 1;

        float scaleFactor = get_scale_factor(rotations[p0].timestamp, rotations[p1].timestamp, dt);
        glm::quat final_rotation = glm::slerp(rotations[p0].orientation, rotations[p1].orientation, scaleFactor);
        final_rotation = glm::normalize(final_rotation);
        return glm::mat4(final_rotation);
    }

    glm::mat4 BoneAnimator::interpolate_scale(float dt) {
        auto& scales = bone->scales;

        if (scales.size() == 1)
            return glm::scale(glm::mat4(1.0f), scales[0].scale);

        int p0 = 0;
        size_t scale_size = scales.size();
        for (int index = 0; index < scale_size - 1; ++index) {
            if (dt < scales[index + 1].timestamp) {
                p0 = index;
                break;
            }
        }
        int p1 = p0 + 1;

        float scale_factor = get_scale_factor(scales[p0].timestamp, scales[p1].timestamp, dt);
        glm::vec3 final_scale = glm::mix(scales[p0].scale, scales[p1].scale, scale_factor);
        return glm::scale(glm::mat4(1.0f), final_scale);
    }

    Animator::Animator(Animation* animation) {
        set_animation(animation);
        bone_transforms.reserve(100);
        for (int i = 0; i < 100; i++)
            bone_transforms.emplace_back(1.0f);
    }

    void Animator::update(float dt) {
        this->dt = dt;
        if (animation && running) {
            current_time += animation->ticks_per_second * dt;
            current_time = fmod(current_time, animation->duration);
            calculate_bone_transforms(&animation->root, glm::mat4(1.0f));
        }
    }

    void Animator::calculate_bone_transforms(const AssimpNodeData* node, glm::mat4 parent_transform) {
        std::string node_name = node->name;
        glm::mat4 node_transform = node->transformation;
        glm::mat4 global_transform;
        auto bone_it = animation->bones->find(node_name);

        if (bone_it != animation->bones->end()) {
            auto* bone = &bone_it->second;
            bone_animator.bone = bone;
            bone_animator.update(current_time);
            global_transform = parent_transform * bone->transform;
            bone_transforms[bone->id] = global_transform * bone->offset;
        } else {
            global_transform = parent_transform * node_transform;
        }

        for (const auto& child : node->children)
            calculate_bone_transforms(&child, global_transform);
    }

    void Animator::set_animation(Animation* animation) {
        this->animation = animation;
        current_time = 0;
    }

    void Animator::play() {
        running = true;
    }

    void Animator::pause() {
        running = false;
    }

    void Animator::stop() {
        running = false;
        current_time = 0;
    }
}