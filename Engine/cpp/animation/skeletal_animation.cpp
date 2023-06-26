#include <animation/skeletal_animation.h>

namespace gl {

    void BoneAnimator::update(float dt) {
        glm::mat4 translation = interpolatePosition(dt);
        glm::mat4 rotation = interpolateRotation(dt);
        glm::mat4 scale = interpolateScale(dt);
        bone->transform = translation * rotation * scale;
    }

    static float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
        return (animationTime - lastTimeStamp) / (nextTimeStamp - lastTimeStamp);
    }

    glm::mat4 BoneAnimator::interpolatePosition(float dt) {
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

        float scale_factor = getScaleFactor(positions[p0].timestamp, positions[p1].timestamp, dt);
        glm::vec3 final_position = glm::mix(positions[p0].position, positions[p1].position, scale_factor);
        return glm::translate(glm::mat4(1.0f), final_position);
    }

    glm::mat4 BoneAnimator::interpolateRotation(float dt) {
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

        float scaleFactor = getScaleFactor(rotations[p0].timestamp, rotations[p1].timestamp, dt);
        glm::quat final_rotation = glm::slerp(rotations[p0].orientation, rotations[p1].orientation, scaleFactor);
        final_rotation = glm::normalize(final_rotation);
        return glm::mat4(final_rotation);
    }

    glm::mat4 BoneAnimator::interpolateScale(float dt) {
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

        float scale_factor = getScaleFactor(scales[p0].timestamp, scales[p1].timestamp, dt);
        glm::vec3 final_scale = glm::mix(scales[p0].scale, scales[p1].scale, scale_factor);
        return glm::scale(glm::mat4(1.0f), final_scale);
    }

    Animator::Animator(Animation* animation) {
        setAnimation(animation);
        boneTransforms.reserve(100);
        for (int i = 0; i < 100; i++)
            boneTransforms.emplace_back(1.0f);
    }

    void Animator::update(float dt) {
        this->dt = dt;
        if (animation && running) {
            currentTime += animation->ticksPerSecond * dt;
            currentTime = fmod(currentTime, animation->duration);
            calculateBoneTransforms(&animation->root, glm::mat4(1.0f));
        }
    }

    void Animator::calculateBoneTransforms(const AssimpNodeData* node, const glm::mat4& parentTransform) {
        std::string nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;
        glm::mat4 globalTransform;
        auto boneIt = animation->bones->find(nodeName);

        if (boneIt != animation->bones->end()) {
            auto* bone = &boneIt->second;
            boneAnimator.bone = bone;
            boneAnimator.update(currentTime);
            globalTransform = parentTransform * bone->transform;
            boneTransforms[bone->id] = globalTransform * bone->offset;
        } else {
            globalTransform = parentTransform * nodeTransform;
        }

        for (const auto& child : node->children)
            calculateBoneTransforms(&child, globalTransform);
    }

    void Animator::setAnimation(Animation* animation) {
        this->animation = animation;
        currentTime = 0;
    }

    void Animator::play() {
        running = true;
    }

    void Animator::pause() {
        running = false;
    }

    void Animator::stop() {
        running = false;
        currentTime = 0;
    }
}