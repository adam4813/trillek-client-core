#ifndef ECS_STATE_SYSTEM_HPP_INCLUDED
#define ECS_STATE_SYSTEM_HPP_INCLUDED

#include <map>
#include <memory>

#include "trillek.hpp"

namespace trillek {

/* Base class used to manage entity state of type T.
 * Each entity has it's own state stored in a mapping of entity ID to
 * state. To use simply inherit from StateSystem, and, in your class's
 * source file, define (in the trillek namespace) the states instance (e.g):
 *
 * // animation.cpp
 * namespace trillek {
 *
 * using graphics::AnimationState;
 * using graphics::AnimationSystem;
 * std::map<id_t, std::shared_ptr<AnimationState>> AnimationSystem::states; // Mapping of entity ID to animation state.
 * std::shared_ptr<AnimationState> AnimationSystem::default_state = std::make_shared<AnimationState>();
 *
 * } // End of trillek
 *
 * Additionally default_state must be instantiated because all types T will
 * have different numbers of arguments for it's construction.
 */
template <typename T>
class ECSStateSystem {
public:
    /**
     * \brief Get the default state.
     *
     * \return std::shared_ptr<State> The default state.
     */
    static std::shared_ptr<T> GetDefaultState() {
        return default_state;
    }

    /**
     * \brief Get the state for the given entity ID.
     *
     * This doesn't create a state if the entity ID doesn't exist.
     * \param[in] const id_t entityID The ID of the entity to get.
     * \return std::shared_ptr<State> The entity's state or nullptr.
     */
    static std::shared_ptr<T> GetState(const id_t entityID) {
        if (states.find(entityID) != states.end()) {
            return states.at(entityID);
        }
        return nullptr;
    }

    /**
     * \brief Set or add a state for the given entity ID.
     *
     * \param[in] const id_t entityID The ID of the entity to add.
     * \param[in] std::shared_ptr<State> The entity's state.
     * \return void
     */
    static void SetState(const id_t entityID, std::shared_ptr<T> state) {
        states[entityID] = state;
    }

    /**
     * \brief Remove the state for the given entity ID.
     *
     * \param[in] const id_t entityID The ID of the entity to remove.
     * \return void
     */
    static void RemoveState(const id_t entityID) {
        states.erase(entityID);
    }
protected:
    static std::shared_ptr<T> default_state; // Default state instance.

    // TODO: Replace this with a weak_ptr to allow pruning?
    static std::map<id_t, std::shared_ptr<T>> states; // Mapping of entity ID to state.
};

} // End of trillek

#endif
