#ifndef ANIMATION_SYSTEM_HPP_INCLUDED
#define ANIMATION_SYSTEM_HPP_INCLUDED

#include <vector>
#include <map>
#include <memory>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "trillek.hpp"
#include "ecs-state-system.hpp"

namespace trillek {
namespace resource {

class MD5Anim;

} // End of resource

namespace graphics {

struct AnimationState {
    AnimationState() : time(0.0f), paused(false) { }
    std::vector<glm::mat4> matrices;
    float time;
    bool paused; // If paused time won't be advanced
};

class AnimationSystem : public ECSStateSystem<AnimationState> {
public:
    AnimationSystem() { }
    /**
     * \brief Updates the current animation based on a change in time.
     *
     * \param[in] float delta The change in time
     * \return void
     */
    void UpdateAnimation(const float delta);

    /**
     * \brief Sets the animation file for this animation.
     *
     * This does a run for the first set of animation from frame 0 to 1 with time 0.
     * \param[in] std::shared_ptr<resource::MD5Anim> file The animation file.
     * \return void
     */
    void SetAnimationFile(std::shared_ptr<resource::MD5Anim> file);

    /**************************/
    /*  Convenience Function  */
    /**************************/
    /**
     * \brief Pauses the animation for the given entity ID
     *
     * \param[in] const id_t entityID The ID of the entity to pause.
     * \return void
     */
    static void Pause(const id_t entityID);

    /**
     * \brief Resumes the animation for the given entity ID
     *
     * \param[in] const id_t entityID The ID of the entity to resume.
     * \return void
     */
    static void Resume(const id_t entityID);

    /**
     * \brief Returns if the given entity's animation is paused.
     *
     * \param[in] const id_t entityID The ID of the entity to check.
     * \return bool true if the entity's animation is paused (Returns false on error).
     */
    static bool IsPaused(const id_t entityID);

    friend class RenderSystem;
private:
    std::shared_ptr<resource::MD5Anim> animation_file; // Animation file resource.

    size_t frame_count; // Number of frames for the given animation.

    float animation_duration; // Length for the entire animation.
    float frame_rate; // Frames per second.
};

} // End of graphics
} // End of trillek

#endif
