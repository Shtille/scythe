#include "physics_hinge_constraint.h"

#include "physics_rigid_body.h"

#include "common/sc_assert.h"
#include "common/sc_delete.h"

namespace scythe {

	void PhysicsHingeConstraint::SetLimits(float minAngle, float maxAngle, float bounciness)
	{
		// Use the defaults for softness (0.9) and biasFactor (0.3).
		SC_ASSERT(constraint_);
		((btHingeConstraint*)constraint_)->setLimit(minAngle, maxAngle, 0.9f, 0.3f, bounciness);
	}

	PhysicsHingeConstraint::PhysicsHingeConstraint(PhysicsRigidBody* a, const Quaternion& rotationOffsetA, const Vector3& translationOffsetA,
		PhysicsRigidBody* b, const Quaternion& rotationOffsetB, const Vector3& translationOffsetB)
	: PhysicsConstraint(a, b)
	{
		SC_ASSERT(a && a->body_ && a->GetNode());

		// Take scale into account for the first node's translation offset.
		Vector3 sA;
		a->GetNode()->GetWorldMatrix().GetScale(&sA);
		Vector3 tA(translationOffsetA.x * sA.x, translationOffsetA.y * sA.y, translationOffsetA.z * sA.z);

		if (b)
		{
			SC_ASSERT(b->body_ && b->GetNode());

			// Take scale into account for the second node's translation offset.
			Vector3 sB;
			b->GetNode()->GetWorldMatrix().GetScale(&sB);
			Vector3 tB(translationOffsetB.x * sB.x, translationOffsetB.y * sB.y, translationOffsetB.z * sB.z);

			btTransform frameInA(BQ(rotationOffsetA), BV(tA));
			btTransform frameInB(BQ(rotationOffsetB), BV(tB));
			constraint_ = new btHingeConstraint(*a->body_, *b->body_, frameInA, frameInB);
		}
		else
		{
			btTransform frameInA(BQ(rotationOffsetA), BV(tA));
			constraint_ = new btHingeConstraint(*a->body_, frameInA);
		}
	}

		
	PhysicsHingeConstraint::~PhysicsHingeConstraint()
	{
		// Unused
	}

} // namespace scythe