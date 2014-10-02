#ifndef TRANSFORMUPDATESYSTEM_HPP_INCLUDED
#define TRANSFORMUPDATESYSTEM_HPP_INCLUDED

#include <map>
#include <mutex>

#include "trillek.hpp"
#include "util/json-parser.hpp"
#include "systems/async-data.hpp"
#include "atomic-map.hpp"

namespace trillek {

class Transform;

namespace physics {

class PhysicsSystem;

} // End of physics

class TransformUpdateSystem : public util::Parser {
private:
    TransformUpdateSystem() : Parser("transforms") { }
    TransformUpdateSystem(const TransformUpdateSystem& right) : Parser("transforms") {
        instance = right.instance;
    }
    TransformUpdateSystem& operator=(const TransformUpdateSystem& right) {
        if (this != &right) {
            instance = right.instance;
        }

        return *this;
    }
    static std::once_flag only_one;
    static std::shared_ptr<TransformUpdateSystem> instance;
public:
    static std::shared_ptr<TransformUpdateSystem> GetInstance() {
        std::call_once(TransformUpdateSystem::only_one,
            [ ] () {
            TransformUpdateSystem::instance.reset(new TransformUpdateSystem());
        }
        );

        return TransformUpdateSystem::instance;
    }
    ~TransformUpdateSystem() { }

    /**
     * \brief Get a Async map of the updated transforms.
     *
     * \return AsyncData<std::map<id_t,const Transform*>>& The updated transform AsyncMap.
     */
    static AsyncData<std::map<id_t,const Transform*>>& GetAsyncUpdatedMap() {
        return instance->async_updated_transforms;
    }

    // TODO: Move this into a separate class/module.
    virtual bool Serialize();
    virtual bool Parse(rapidjson::Value& node);
    std::map<id_t, rapidjson::Value*> source_nodes; // Stores the transforms original source node.
private:
    friend class physics::PhysicsSystem;
    friend class Transform;

    /**
     * \brief Get a Map for the updated transforms.
     *
     * This is used internally by physics to publish the list of updated transforms.
     * \return AtomicMap<id_t, const Transform*>& The updated transform map.
     */
    static AtomicMap<id_t, const Transform*>& GetUpdatedMap() {
        return instance->updated_transforms;
    };

    AtomicMap<id_t,const Transform*> updated_transforms;
    AsyncData<std::map<id_t,const Transform*>> async_updated_transforms;
};

} // End of trillek

#endif
