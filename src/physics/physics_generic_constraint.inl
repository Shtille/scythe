#include "physics_generic_constraint.h"

#include "common/sc_assert.h"

namespace scythe {

	inline const Quaternion& PhysicsGenericConstraint::GetRotationOffsetA() const
	{
		if (!rotation_offset_a_)
			rotation_offset_a_ = new Quaternion();

		SC_ASSERT(constraint_);
		btQuaternion ro = static_cast<btGeneric6DofConstraint*>(constraint_)->getFrameOffsetA().getRotation();
		rotation_offset_a_->set(ro.x(), ro.y(), ro.z(), ro.w());
		return *rotation_offset_a_;
	}

	inline const Quaternion& PhysicsGenericConstraint::GetRotationOffsetB() const
	{
		if (!rotation_offset_b_)
			rotation_offset_b_ = new Quaternion();

		SC_ASSERT(constraint_);
		btQuaternion ro = static_cast<btGeneric6DofConstraint*>(constraint_)->getFrameOffsetB().getRotation();
		rotation_offset_b_->set(ro.x(), ro.y(), ro.z(), ro.w());
		return *rotation_offset_b_;
	}

	inline const Vector3& PhysicsGenericConstraint::GetTranslationOffsetA() const
	{
		if (!translation_offset_a_)
			translation_offset_a_ = new Vector3();

		SC_ASSERT(constraint_);
		btVector3 to = static_cast<btGeneric6DofConstraint*>(constraint_)->getFrameOffsetA().getOrigin();
		translation_offset_a_->set(to.x(), to.y(), to.z());
		return *translation_offset_a_;
	}

	inline const Vector3& PhysicsGenericConstraint::GetTranslationOffsetB() const
	{
		if (!translation_offset_b_)
			translation_offset_b_ = new Vector3();

		SC_ASSERT(constraint_);
		btVector3 to = static_cast<btGeneric6DofConstraint*>(constraint_)->getFrameOffsetB().getOrigin();
		translation_offset_b_->set(to.x(), to.y(), to.z());
		return *translation_offset_b_;
	}

	inline void PhysicsGenericConstraint::SetAngularLowerLimit(const Vector3& limits)
	{
		SC_ASSERT(constraint_);
		((btGeneric6DofConstraint*)constraint_)->setAngularLowerLimit(BV(limits));
	}

	inline void PhysicsGenericConstraint::SetAngularUpperLimit(const Vector3& limits)
	{
		SC_ASSERT(constraint_);
		((btGeneric6DofConstraint*)constraint_)->setAngularUpperLimit(BV(limits));
	}

	inline void PhysicsGenericConstraint::SetLinearLowerLimit(const Vector3& limits)
	{
		SC_ASSERT(constraint_);
		((btGeneric6DofConstraint*)constraint_)->setLinearLowerLimit(BV(limits));
	}
		
	inline void PhysicsGenericConstraint::SetLinearUpperLimit(const Vector3& limits)
	{
		SC_ASSERT(constraint_);
		((btGeneric6DofConstraint*)constraint_)->setLinearUpperLimit(BV(limits));
	}

	inline void PhysicsGenericConstraint::SetRotationOffsetA(const Quaternion& rotationOffset)
	{
		SC_ASSERT(constraint_);
		static_cast<btGeneric6DofConstraint*>(constraint_)->getFrameOffsetA().setRotation(BQ(rotationOffset));
	}

	inline void PhysicsGenericConstraint::SetRotationOffsetB(const Quaternion& rotationOffset)
	{
		SC_ASSERT(constraint_);
		static_cast<btGeneric6DofConstraint*>(constraint_)->getFrameOffsetB().setRotation(BQ(rotationOffset));
	}

	inline void PhysicsGenericConstraint::SetTranslationOffsetA(const Vector3& translationOffset)
	{
		SC_ASSERT(constraint_);
		static_cast<btGeneric6DofConstraint*>(constraint_)->getFrameOffsetA().setOrigin(BV(translationOffset));
	}

	inline void PhysicsGenericConstraint::SetTranslationOffsetB(const Vector3& translationOffset)
	{
		SC_ASSERT(constraint_);
		static_cast<btGeneric6DofConstraint*>(constraint_)->getFrameOffsetB().setOrigin(BV(translationOffset));
	}

} // namespace scythe