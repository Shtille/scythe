#include "physics_ghost_object.h"

#include "physics_controller.h"

#include "bullet_include.h"

#include "common/sc_assert.h"
#include "common/sc_delete.h"

namespace scythe {

	PhysicsGhostObject::PhysicsGhostObject(Node* node, const PhysicsCollisionShape::Definition& shape, int group, int mask)
	: PhysicsCollisionObject(node, group, mask)
	, ghost_object_(NULL)
	{
		Vector3 centerOfMassOffset;
		PhysicsController * controller = PhysicsController::GetInstance();
		SC_ASSERT(controller);

		// Create and set the collision shape for the ghost object.
		collision_shape_ = controller->CreateShape(node, shape, &centerOfMassOffset, false);
		SC_ASSERT(collision_shape_);

		// Create the ghost object.
		ghost_object_ = new btPairCachingGhostObject();
		ghost_object_->setCollisionShape(collision_shape_->shape());
		ghost_object_->setCollisionFlags(ghost_object_->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

		// Initialize a physics motion state object for syncing the transform.
		motion_state_ = new PhysicsMotionState(node_, this, &centerOfMassOffset);
		motion_state_->getWorldTransform(ghost_object_->getWorldTransform());

		// Add the ghost object to the physics world.
		controller->AddCollisionObject(this);

		SC_ASSERT(node_);
		node_->AddListener(this);
	}

	PhysicsGhostObject::~PhysicsGhostObject()
	{
		SC_ASSERT(node_);
		node_->RemoveListener(this);

		PhysicsController * controller = PhysicsController::GetInstance();
		SC_ASSERT(controller);
		controller->RemoveCollisionObject(this, true);

		SC_SAFE_DELETE(ghost_object_);
	}

	PhysicsCollisionObject::Type PhysicsGhostObject::type() const
	{
		return kGhostObject;
	}

	btCollisionObject* PhysicsGhostObject::GetCollisionObject() const
	{
		return ghost_object_;
	}

	void PhysicsGhostObject::TransformChanged(Transform* transform, long cookie)
	{
		SC_ASSERT(motion_state_);
		SC_ASSERT(ghost_object_);

		// Update the motion state with the transform from the node.
		motion_state_->UpdateTransformFromNode();

		// Update the transform on the ghost object.
		motion_state_->getWorldTransform(ghost_object_->getWorldTransform());
	}

} // namespace scythe