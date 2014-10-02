#include "ecs-state-system.hpp"
#include "transform.hpp"

namespace trillek {
    std::map<id_t, std::shared_ptr<Transform>> ECSStateSystem<Transform>::states;
    std::shared_ptr<Transform> ECSStateSystem<Transform>::default_state = std::make_shared<Transform>(0);
} // End of trillek
