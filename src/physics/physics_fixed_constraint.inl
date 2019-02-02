#include "physics_fixed_constraint.h"

namespace scythe {

	inline void PhysicsFixedConstraint::SetAngularLowerLimit(const Vector3& limit)
	{
		PhysicsGenericConstraint::SetAngularLowerLimit(limit);
	}

	inline void PhysicsFixedConstraint::SetAngularUpperLimit(const Vector3& limit)
	{
		PhysicsGenericConstraint::SetAngularUpperLimit(limit);
	}

	inline void PhysicsFixedConstraint::SetLinearLowerLimit(const Vector3& limit)
	{
		PhysicsGenericConstraint::SetLinearLowerLimit(limit);
	}

	inline void PhysicsFixedConstraint::SetLinearUpperLimit(const Vector3& limit)
	{
		PhysicsGenericConstraint::SetLinearUpperLimit(limit);
	}

} // namespace scythe