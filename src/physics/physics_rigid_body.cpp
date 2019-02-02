#include "physics_rigid_body.h"

#include "physics_controller.h"

#include "common/sc_assert.h"
#include "common/sc_delete.h"

namespace scythe {

	PhysicsRigidBody::PhysicsRigidBody(Node * node, const PhysicsCollisionShape::Definition& shape, const Parameters& parameters, int group, int mask)
	: PhysicsCollisionObject(node, group, mask)
	, body_(NULL)
	, mass_(parameters.mass)
	, constraints_(NULL)
	, in_destructor_(false)
	{
		PhysicsController * controller = PhysicsController::GetInstance();
		SC_ASSERT(controller);
		SC_ASSERT(node_);

		// Create our collision shape.
		Vector3 centerOfMassOffset;
		collision_shape_ = controller->CreateShape(node, shape, &centerOfMassOffset, parameters.mass != 0.0f);
		SC_ASSERT(collision_shape_ && collision_shape_->shape());

		// Create motion state object.
		motion_state_ = new PhysicsMotionState(node, this, (centerOfMassOffset.Sqr() > MATH_EPSILON) ? &centerOfMassOffset : NULL);

		// If the mass is non-zero, then the object is dynamic so we calculate the local 
		// inertia. However, if the collision shape is a triangle mesh, we don't calculate 
		// inertia since Bullet doesn't currently support this.
		btVector3 localInertia(0.0, 0.0, 0.0);
		if (parameters.mass != 0.0)
			collision_shape_->shape()->calculateLocalInertia(parameters.mass, localInertia);

		// Create the Bullet physics rigid body object.
		btRigidBody::btRigidBodyConstructionInfo rbInfo(parameters.mass, NULL, collision_shape_->shape(), localInertia);
		rbInfo.m_friction = parameters.friction;
		rbInfo.m_restitution = parameters.restitution;
		rbInfo.m_linearDamping = parameters.linearDamping;
		rbInfo.m_angularDamping = parameters.angularDamping;

		// Create + assign the new bullet rigid body object.
		body_ = new btRigidBody(rbInfo);

		// Set motion state after rigid body assignment, since bullet will callback on the motion state interface to query
		// the initial transform and it will need to access to rigid body (body_).
		body_->setMotionState(motion_state_);

		// Set other initially defined properties.
		SetKinematic(parameters.kinematic);
		SetAnisotropicFriction(parameters.anisotropicFriction);
		SetAngularFactor(parameters.angularFactor);
		SetLinearFactor(parameters.linearFactor);

		// Add ourself to the physics world.
		controller->AddCollisionObject(this);

		if (collision_shape_->type() == PhysicsCollisionShape::kHeightfield)
		{
			// Add a listener on the node's transform so we can track dirty changes to calculate
			// an inverse matrix for transforming heightfield points between world and local space.
			node_->AddListener(this);
		}
	}
	PhysicsRigidBody::~PhysicsRigidBody()
	{
		PhysicsController * controller = PhysicsController::GetInstance();
		SC_ASSERT(controller);
		SC_ASSERT(collision_shape_);
		SC_ASSERT(node_);

		// Clean up all constraints linked to this rigid body.
		in_destructor_ = true;
		if (constraints_)
		{
			for (unsigned int i = 0; i < constraints_->size(); ++i)
			{
				SC_SAFE_DELETE((*constraints_)[i]);
			}
			SC_SAFE_DELETE(constraints_);
		}

		// Remove collision object from physics controller.
		controller->RemoveCollisionObject(this, true);

		// Clean up the rigid body and its related objects.
		SC_SAFE_DELETE(body_);

		// Unregister node listener (only registered for heihgtfield collision shape types).
		if (collision_shape_->type() == PhysicsCollisionShape::kHeightfield)
		{
			node_->RemoveListener(this);
		}
	}
	PhysicsCollisionObject::Type PhysicsRigidBody::type() const
	{
		return PhysicsCollisionObject::kRigidBody;
	}
	btCollisionObject * PhysicsRigidBody::GetCollisionObject() const
	{
		return body_;
	}
	void PhysicsRigidBody::ApplyForce(const Vector3& force, const Vector3* relativePosition)
	{
		// If the force is significant enough, activate the rigid body 
		// to make sure that it isn't sleeping and apply the force.
		if (force.Sqr() > MATH_EPSILON)
		{
			SC_ASSERT(body_);
			body_->activate();
			if (relativePosition)
				body_->applyForce(BV(force), BV(*relativePosition));
			else
				body_->applyCentralForce(BV(force));
		}
	}
	void PhysicsRigidBody::ApplyImpulse(const Vector3& impulse, const Vector3* relativePosition)
	{
		// If the impulse is significant enough, activate the rigid body 
		// to make sure that it isn't sleeping and apply the impulse.
		if (impulse.Sqr() > MATH_EPSILON)
		{
			SC_ASSERT(body_);
			body_->activate();
			if (relativePosition)
			{
				body_->applyImpulse(BV(impulse), BV(*relativePosition));
			}
			else
				body_->applyCentralImpulse(BV(impulse));
		}
	}
	void PhysicsRigidBody::ApplyTorque(const Vector3& torque)
	{
		// If the torque is significant enough, activate the rigid body 
		// to make sure that it isn't sleeping and apply the torque.
		if (torque.Sqr() > MATH_EPSILON)
		{
			SC_ASSERT(body_);
			body_->activate();
			body_->applyTorque(BV(torque));
		}
	}
	void PhysicsRigidBody::ApplyTorqueImpulse(const Vector3& torque)
	{
		// If the torque impulse is significant enough, activate the rigid body 
		// to make sure that it isn't sleeping and apply the torque impulse.
		if (torque.Sqr() > MATH_EPSILON)
		{
			SC_ASSERT(body_);
			body_->activate();
			body_->applyTorqueImpulse(BV(torque));
		}
	}
	void PhysicsRigidBody::SetKinematic(bool kinematic)
	{
		SC_ASSERT(body_);

		if (kinematic)
		{
			body_->setCollisionFlags(body_->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			body_->setActivationState(DISABLE_DEACTIVATION);
		}
		else
		{
			body_->setCollisionFlags(body_->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
			body_->setActivationState(ACTIVE_TAG);
		}
	}
	void PhysicsRigidBody::SetEnabled(bool enable)
	{
		PhysicsCollisionObject::SetEnabled(enable);
		if (enable)
			body_->setMotionState(motion_state_);
	}
	void PhysicsRigidBody::AddConstraint(PhysicsConstraint * constraint)
	{
		SC_ASSERT(constraint);
		if (constraints_ == NULL)
			constraints_ = new std::vector<PhysicsConstraint*>();

		constraints_->push_back(constraint);
	}
	void PhysicsRigidBody::RemoveConstraint(PhysicsConstraint * constraint)
	{
		// Ensure that the rigid body has constraints and that we are
		// not currently in the rigid body's destructor (in this case,
		// the constraints will be destroyed from there).
		if (constraints_ && !in_destructor_)
		{
			for (unsigned int i = 0; i < constraints_->size(); ++i)
			{
				if ((*constraints_)[i] == constraint)
				{
					constraints_->erase(constraints_->begin() + i);
					break;
				}
			}
		}
	}
	bool PhysicsRigidBody::SupportsConstraints()
	{
		return (GetShapeType() != PhysicsCollisionShape::kHeightfield &&
				GetShapeType() != PhysicsCollisionShape::kMesh);
	}

} // namespace scythe

#endif