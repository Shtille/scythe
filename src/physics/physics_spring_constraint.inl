#include "physics_spring_constraint.h"

namespace scythe {

	inline void PhysicsSpringConstraint::SetAngularDampingX(float damping)
	{
		SetDamping(ANGULAR_X, damping);
	}

	inline void PhysicsSpringConstraint::SetAngularDampingY(float damping)
	{
		SetDamping(ANGULAR_Y, damping);
	}

	inline void PhysicsSpringConstraint::SetAngularDampingZ(float damping)
	{
		SetDamping(ANGULAR_Z, damping);
	}

	inline void PhysicsSpringConstraint::SetAngularStrengthX(float strength)
	{
		SetStrength(ANGULAR_X, strength);
	}

	inline void PhysicsSpringConstraint::SetAngularStrengthY(float strength)
	{
		SetStrength(ANGULAR_Y, strength);
	}

	inline void PhysicsSpringConstraint::SetAngularStrengthZ(float strength)
	{
		SetStrength(ANGULAR_Z, strength);
	}

	inline void PhysicsSpringConstraint::SetLinearDampingX(float damping)
	{
		SetDamping(LINEAR_X, damping);
	}

	inline void PhysicsSpringConstraint::SetLinearDampingY(float damping)
	{
		SetDamping(LINEAR_Y, damping);
	}

	inline void PhysicsSpringConstraint::SetLinearDampingZ(float damping)
	{
		SetDamping(LINEAR_Z, damping);
	}

	inline void PhysicsSpringConstraint::SetLinearStrengthX(float strength)
	{
		SetStrength(LINEAR_X, strength);
	}

	inline void PhysicsSpringConstraint::SetLinearStrengthY(float strength)
	{
		SetStrength(LINEAR_Y, strength);
	}

	inline void PhysicsSpringConstraint::SetLinearStrengthZ(float strength)
	{
		SetStrength(LINEAR_Z, strength);
	}

} // namespace scythe