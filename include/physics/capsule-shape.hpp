#ifndef CAPSULE_SHAPE_HPP
#define CAPSULE_SHAPE_HPP

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <memory>

#include "systems/component-factory.hpp"

namespace trillek {

class Transform;

namespace physics {

class CapsuleShape : public ComponentBase {
public:
    CapsuleShape() { }
    ~CapsuleShape() {
        if (this->motion_state) {
            delete this->motion_state;
        }
    }

    /**
    * \brief Initializes the component with the provided properties
    *
    * Valid properties include mesh (the mesh resource name) and shader (the shader resource name).
    * \param[in] const std::vector<Property>& properties The creation properties for the component.
    * \return bool True if initialization finished with no errors.
    */
    bool Initialize(const std::vector<Property> &properties);

    /**
    * \brief Sets the shapes transform using the provided entity ID
    *
    * \param[in] unsigned int entity_id The ID of the entity to get the transform for.
    * \return void
    */
    void SetEntity(unsigned int entity_id);

    /**
    * \brief Initilizes the rigied body after the shape has been initialized.
    *
    * \return void
    */
    void InitializeRigidBody();

    /**
    * \brief Gets the shape's rigidbody.
    *
    * \return btRigidBody* The shape's rigidbody.
    */
    btRigidBody* GetRigidBody() const { return this->body.get(); };

    /**
    * \brief Updates the entity's transform with the current motion_state.
    *
    * \return void
    */
    void UpdateTransform();
private:
    double radius;
    double height;
    btMotionState* motion_state;

    std::unique_ptr<btCollisionShape> shape;
    std::unique_ptr<btRigidBody> body;

    std::shared_ptr<Transform> entity_transform;
};

} // End of physics
namespace reflection {

template <> inline const char* GetTypeName<physics::CapsuleShape>() { return "capsule"; }
template <> inline const unsigned int GetTypeID<physics::CapsuleShape>() { return 3000; }

} // End of reflection
} // End of trillek

#endif