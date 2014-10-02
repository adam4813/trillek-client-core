#include "graphics/animation-system.hpp"

#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "resources/md5anim.hpp"

namespace trillek {

using graphics::AnimationState;
template<>
std::map<id_t, std::shared_ptr<AnimationState>> ECSStateSystem<AnimationState>::states = { }; // Mapping of entity ID to animation state.
template<>
std::shared_ptr<AnimationState> ECSStateSystem<AnimationState>::default_state = std::make_shared<AnimationState>();

namespace graphics {

void AnimationSystem::UpdateAnimation(const float delta) {
    if (this->frame_count < 1) {
        return;
    }

    for (const auto state : states) {
        if (state.second->paused) {
            continue;
        }
        float& time = state.second->time;

        time += delta;

        while (time > this->animation_duration) {
            time -= this->animation_duration;
        }
        while (time < 0.0f) {
            time += this->animation_duration;
        }

        // Figure out which frame we're on
        float frame_number = time * frame_rate;
        int frame_index0 = (int)floorf(frame_number);
        int frame_index1 = (int)ceilf(frame_number);
        frame_index0 = frame_index0 % frame_count;
        frame_index1 = frame_index1 % frame_count;

        float fInterpolate = fmodf(time, this->animation_duration);

        if (this->animation_file) {
            auto frame_skeleton = this->animation_file->InterpolateSkeletons(
                frame_index0, frame_index1, fInterpolate);
            state.second->matrices.assign(frame_skeleton.bone_matricies.begin(),
                frame_skeleton.bone_matricies.end());
        }
    }
}

void AnimationSystem::SetAnimationFile(std::shared_ptr<resource::MD5Anim> file) {
    if (file) {
        this->animation_file = file;
        this->frame_count = this->animation_file->GetFrameCount();
        this->frame_rate = static_cast<float>(this->animation_file->GetFrameRate());
        this->animation_duration = 1.0f / this->frame_rate * this->frame_count;

        auto frame_skeleton = this->animation_file->InterpolateSkeletons(
            0, 1, 0.0f);
        this->default_state->matrices.assign(frame_skeleton.bone_matricies.begin(),
            frame_skeleton.bone_matricies.end());
    }
}
void AnimationSystem::Pause(const id_t entityID) {
    if (states.find(entityID) != states.end()) {
        states[entityID]->paused = true;
    }
}

void AnimationSystem::Resume(const id_t entityID) {
    if (states.find(entityID) != states.end()) {
        states[entityID]->paused = false;
    }
}

bool AnimationSystem::IsPaused(const id_t entityID) {
    if (states.find(entityID) != states.end()) {
        return states[entityID]->paused;
    }
    return false;
}

} // End of graphics
} // End of trillek
