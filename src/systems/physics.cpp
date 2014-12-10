#include "trillek-game.hpp"
#include "components/shared-component.hpp"
#include "components/system-component.hpp"
#include "components/system-component-value.hpp"
#include "physics/collidable.hpp"
#include "systems/transform-system.hpp"
#include <bullet/BulletCollision/Gimpact/btGImpactShape.h>
#include <bullet/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h>
#include "logging.hpp"
#include "user-command.hpp"

namespace trillek { namespace physics {

using namespace component;

PhysicsSystem::PhysicsSystem() { }
PhysicsSystem::~PhysicsSystem() { }

void PhysicsSystem::Start() {
    this->collisionConfiguration = new btDefaultCollisionConfiguration();
    this->dispatcher = new btCollisionDispatcher(this->collisionConfiguration);
    this->broadphase = new btDbvtBroadphase();
    this->solver = new btSequentialImpulseConstraintSolver();
    this->dynamicsWorld = new btDiscreteDynamicsWorld(this->dispatcher, this->broadphase, this->solver, this->collisionConfiguration);
    this->dynamicsWorld->setGravity(btVector3(0, -6.7, 0));

    // Register the collision dispatcher with the GImpact algorithm for dynamic meshes.
    btCollisionDispatcher * dispatcher = static_cast<btCollisionDispatcher *>(this->dynamicsWorld->getDispatcher());
    btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);
}

void PhysicsSystem::HandleEvents(frame_tp timepoint) {
    // Execute the commands
    auto iterator_pair = this->usercommands.GetAndTagCommandsFrom(timepoint);
    for (auto& v = iterator_pair.first; v != iterator_pair.second; ++v) {
        usercommand::Execute(v->second.first, std::move(v->second.second));
    }
    // commit velocity updates
    Commit<Component::Velocity>(timepoint);

    static frame_tp last_tp;
    this->delta = timepoint - last_tp;
    last_tp = timepoint;

    // Set the rigid bodies linear velocity. Must be done each frame otherwise,
    // other forces will stop the linear velocity.
    // We use the published list

    // First moving entities that have no combined velocity
    OnTrue(GetLastPositiveBitMap<Component::Velocity>()
            & ~Bitmap<Component::ReferenceFrame>(),
        [](id_t entity_id) {
            // first inject velocity of entities that have no reference frame
            auto& body = *Get<Component::Collidable>(entity_id).GetRigidBody();
            const auto& v = Get<Component::Velocity>(entity_id);

            body.applyCentralImpulse(v.GetLinear());
            body.setAngularVelocity(v.GetAngular());
        }
    );
    // Second moving entities with a combined velocity
    OnTrue(GetLastPositiveBitMap<Component::Velocity>()
            & Bitmap<Component::ReferenceFrame>(),
        [](id_t entity_id) {
            // combine velocity
            auto reference_id = Get<Component::ReferenceFrame>(entity_id);
            const auto& v = Get<Component::Velocity>(entity_id);
            const auto& ref_v = Get<Component::Velocity>(reference_id);
            auto& body = *Get<Component::Collidable>(reference_id).GetRigidBody();
            auto& transform = body.getCenterOfMassTransform();
            auto combined_l = transform * v.GetLinear();
            combined_l += ref_v.GetLinear();
            body.setLinearVelocity(combined_l + body.getGravity());
            auto combined_a = transform * v.GetAngular();
            combined_a += ref_v.GetAngular();
            body.setAngularVelocity(combined_a);
        }
    );

    OnTrue(Bitmap<Component::Movable>() & Bitmap<Component::ReferenceFrame>(),
        [](id_t entity_id) {
            if(!Get<Component::Movable>(entity_id)) return;
            auto reference_id = Get<Component::ReferenceFrame>(entity_id);
            auto& body = *Get<Component::Collidable>(entity_id).GetRigidBody();
            auto& rbody = *Get<Component::Collidable>(reference_id).GetRigidBody();
            auto& reftransform = rbody.getWorldTransform();
            auto npq = reftransform * btQuaternion(0,0,0,1.);
            auto npl = reftransform * btVector3(0,0,-1.5);
            npl.setX(((long)(npl.x() * 20.0)) * 0.05);
            npl.setY(((long)(npl.y() * 20.0)) * 0.05);
            npl.setZ(((long)(npl.z() * 20.0)) * 0.05);
            auto ts = btTransform(btQuaternion(0,0,0,1.), npl);
            body.setWorldTransform(ts);
        }
    );

    // Third, entities with reference frame that have moved
    OnTrue(GetLastPositiveBitMap<Component::Velocity>()
            & Bitmap<Component::IsReferenceFrame>(),
        [&](id_t entity_id) {
            // todo
        }
    );

    auto not_immune = ~Bitmap<Component::Immune>();

    // display a message for entities with health < 10
    OnTrue(Lower<Component::Health>(10) & not_immune,
        [](id_t id) {
            LOGMSG(INFO) << "Entity #" << id << " health under 10 (" << Get<Component::Health>(id) << ")";
        }
    );

    // Kill entities with health and whose health is 0 and are not immune
    OnTrue(Bitmap<Component::Health>() & not_immune,
        [](id_t entity_id) {
            // this function is executed only on entitities that has a health component
            auto health = Get<Component::Health>(entity_id);
            if (health == 0) {
                //kill entity
                LOGMSG(INFO) << "Entity #" << entity_id << " should die now";
                // set helth to 300
                Update<Component::Health>(entity_id, 300);
                // set immunity
                Insert<Component::Immune>(entity_id, true);
            }
        }
    );

    // Substract 1 to health of all entities that have not 0
    Add<Component::Health>(-1, NotEqual<Component::Health>(0) & not_immune);

    dynamicsWorld->stepSimulation(delta * 1.0E-9, 10);
    // Set out transform updates.
    auto& bodymap = game.GetSystemComponent().Map<Component::Collidable>();
    for (auto& shape : bodymap) {
        btTransform transform =
            Get<Component::Collidable>(shape.second)->GetRigidBody()->getWorldTransform();

        auto pos = transform.getOrigin();
        glm::vec3 vpos = glm::vec3(pos.x(), pos.y(), pos.z());
        auto rot = transform.getRotation();
        glm::quat vori = glm::quat(rot.w(), rot.x(), rot.y(), rot.z());
        GraphicTransform_type entity_transform(Get<Component::GraphicTransform>(shape.first));
        entity_transform.SetTranslation(vpos);
        entity_transform.SetOrientation(vori);
        Update<Component::GraphicTransform>(shape.first, std::move(entity_transform));
    }

    // Publish the new updated transforms map
    {
        std::lock_guard<std::mutex> tslock(game.transforms_lock);
        Commit<Component::GraphicTransform>(timepoint);
    }
}

void PhysicsSystem::AddDynamicComponent(const unsigned int entity_id, std::shared_ptr<Container> component) {
    if (component->Is<Component::Collidable>()) {
        AddBodyToWorld(component::Get<Component::Collidable>(component)->GetRigidBody());
    }
}

void PhysicsSystem::AddBodyToWorld(btRigidBody* body) {
    this->dynamicsWorld->addRigidBody(body);
}

void PhysicsSystem::Terminate() {
    if (this->dynamicsWorld != nullptr) {
        delete this->dynamicsWorld;
    }
    if (this->solver != nullptr) {
        delete this->solver;
    }
    if (this->collisionConfiguration != nullptr) {
        delete this->collisionConfiguration;
    }
    if (this->dispatcher != nullptr) {
        delete this->dispatcher;
    }
    if (this->broadphase != nullptr) {
        delete this->broadphase;
    }
}

void PhysicsSystem::SetGravity(const unsigned int entity_id, const btVector3& f) {
    auto& system = game.GetSystemComponent();
    if (system.Has<Component::Collidable>(entity_id)) {
        system.Get<Component::Collidable>(entity_id).GetRigidBody()->setGravity(f);
    }
}

void PhysicsSystem::SetNormalGravity(const unsigned int entity_id) {
    auto& system = game.GetSystemComponent();
    if (system.Has<Component::Collidable>(entity_id)) {
        system.Get<Component::Collidable>(entity_id).GetRigidBody()->setGravity(this->dynamicsWorld->getGravity());
    }
}

} // End of physics
} // End of trillek
