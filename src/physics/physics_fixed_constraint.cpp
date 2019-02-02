#include "physics_fixed_constraint.h"

namespace scythe {

	PhysicsFixedConstraint::PhysicsFixedConstraint(PhysicsRigidBody* a, PhysicsRigidBody* b)
	: PhysicsGenericConstraint(a, b)
	{
		PhysicsGenericConstraint::SetAngularLowerLimit(Vector3(0.0f, 0.0f, 0.0f));
		PhysicsGenericConstraint::SetAngularUpperLimit(Vector3(0.0f, 0.0f, 0.0f));
		PhysicsGenericConstraint::SetLinearLowerLimit(Vector3(0.0f, 0.0f, 0.0f));
		PhysicsGenericConstraint::SetLinearUpperLimit(Vector3(0.0f, 0.0f, 0.0f));
	}

	PhysicsFixedConstraint::~PhysicsFixedConstraint()
	{
		// Not used.
	}

} // namespace scythe