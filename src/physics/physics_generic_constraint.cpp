#include "physics_generic_constraint.h"

#include "physics_rigid_body.h"

#include "common/sc_assert.h"
#include "common/sc_delete.h"

namespace scythe {

	PhysicsGenericConstraint::PhysicsGenericConstraint()
	: PhysicsConstraint(NULL, NULL)
	, rotation_offset_a_(NULL)
	, rotation_offset_b_(NULL)
	, translation_offset_a_(NULL)
	, translation_offset_b_(NULL)
	{
		// Not used.
	}

	PhysicsGenericConstraint::PhysicsGenericConstraint(PhysicsRigidBody* a, PhysicsRigidBody* b)
	: PhysicsConstraint(a, b)
	, rotation_offset_a_(NULL)
	, rotation_offset_b_(NULL)
	, translation_offset_a_(NULL)
	, translation_offset_b_(NULL)
	{
		SC_ASSERT(a && a->body_ && a->GetNode());

		if (b)
		{
			SC_ASSERT(b->body_ && b->GetNode());
			Vector3 origin = centerOfMassMidpoint(a->GetNode(), b->GetNode());
			constraint_ = bullet_new<btGeneric6DofConstraint>(*a->body_, *b->body_, getTransformOffset(a->GetNode(), origin), getTransformOffset(b->GetNode(), origin), true);
		}
		else
		{
			constraint_ = bullet_new<btGeneric6DofConstraint>(*a->body_, btTransform::getIdentity(), true);
		}
	}

	PhysicsGenericConstraint::PhysicsGenericConstraint(PhysicsRigidBody* a, const Quaternion& rotationOffsetA, const Vector3& translationOffsetA,
		PhysicsRigidBody* b, const Quaternion& rotationOffsetB, const Vector3& translationOffsetB)
	: PhysicsConstraint(a, b)
	, rotation_offset_a_(NULL)
	, rotation_offset_b_(NULL)
	, translation_offset_a_(NULL)
	, translation_offset_b_(NULL)
	{
		SC_ASSERT(a && a->body_ && a->GetNode());

		// Take scale into account for the first node's translation offset.
		Vector3 sA;
		a->GetNode()->getWorldMatrix().getScale(&sA);
		Vector3 tA(translationOffsetA.x * sA.x, translationOffsetA.y * sA.y, translationOffsetA.z * sA.z);

		if (b)
		{
			SC_ASSERT(b->body_ && b->GetNode());

			// Take scale into account for the second node's translation offset.
			Vector3 sB;
			b->GetNode()->getWorldMatrix().getScale(&sB);
			Vector3 tB(translationOffsetB.x * sB.x, translationOffsetB.y * sB.y, translationOffsetB.z * sB.z);

			btTransform frameInA(BQ(rotationOffsetA), BV(tA));
			btTransform frameInB(BQ(rotationOffsetB), BV(tB));
			constraint_ = bullet_new<btGeneric6DofConstraint>(*a->body_, *b->body_, frameInA, frameInB, true);
		}
		else
		{
			btTransform frameInA(BQ(rotationOffsetA), BV(tA));
			constraint_ = bullet_new<btGeneric6DofConstraint>(*a->body_, frameInA, true);
		}
	}

	PhysicsGenericConstraint::~PhysicsGenericConstraint()
	{
		SC_SAFE_DELETE(rotation_offset_a_);
		SC_SAFE_DELETE(rotation_offset_b_);
		SC_SAFE_DELETE(translation_offset_a_);
		SC_SAFE_DELETE(translation_offset_b_);
	}

} // namespace scythe