#include "physics_constraint.h"

namespace scythe {

	inline float scythe::PhysicsConstraint::GetBreakingImpulse() const
	{
		return constraint_->getBreakingImpulseThreshold();
	}

	inline void PhysicsConstraint::SetBreakingImpulse(float impulse)
	{
		constraint_->setBreakingImpulseThreshold(impulse);
	}

	inline bool PhysicsConstraint::IsEnabled() const
	{
		return constraint_->isEnabled();
	}

	inline void PhysicsConstraint::SetEnabled(bool enabled)
	{
		constraint_->setEnabled(enabled);
	}

} // namespace scythe