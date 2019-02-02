#include "physics_spring_constraint.h"

#include "physics_rigid_body.h"

#include "common/sc_assert.h"

namespace scythe {

	PhysicsSpringConstraint::PhysicsSpringConstraint(PhysicsRigidBody* a, PhysicsRigidBody* b)
	{
		SC_ASSERT(a && a->body_);
		SC_ASSERT(b && b->body_);

		// Initialize the physics rigid body references since we don't call the PhysicsConstraint constructor that does it properly automatically.
		a_ = a;
		b_ = b;

		Vector3 origin = centerOfMassMidpoint(a->GetNode(), b->GetNode());
		constraint_ = new btGeneric6DofSpringConstraint(*a->body_, *b->body_, getTransformOffset(a->GetNode(), origin), getTransformOffset(b->GetNode(), origin), true);
	}

	PhysicsSpringConstraint::PhysicsSpringConstraint(PhysicsRigidBody* a, const Quaternion& rotationOffsetA, const Vector3& translationOffsetA,
													 PhysicsRigidBody* b, const Quaternion& rotationOffsetB, const Vector3& translationOffsetB)
	{
		SC_ASSERT(a && a->body_ && a->GetNode());
		SC_ASSERT(b && b->body_ && b->GetNode());

		// Initialize the physics rigid body references since we don't call the PhysicsConstraint constructor that does it properly automatically.
		a_ = a;
		b_ = b;

		// Take scale into account for the translation offsets.
		Vector3 sA;
		a->GetNode()->getWorldMatrix().getScale(&sA);
		Vector3 tA(translationOffsetA.x * sA.x, translationOffsetA.y * sA.y, translationOffsetA.z * sA.z);

		Vector3 sB;
		b->GetNode()->getWorldMatrix().getScale(&sB);
		Vector3 tB(translationOffsetB.x * sB.x, translationOffsetB.y * sB.y, translationOffsetB.z * sB.z);

		btTransform frameInA(BQ(rotationOffsetA), BV(tA));
		btTransform frameInB(BQ(rotationOffsetB), BV(tB));
		constraint_ = new btGeneric6DofSpringConstraint(*a->body_, *b->body_, frameInA, frameInB, true);
	}

	PhysicsSpringConstraint::~PhysicsSpringConstraint()
	{
		// Used
	}

	void PhysicsSpringConstraint::SetStrength(SpringProperty property, float strength)
	{
		SC_ASSERT(constraint_);
		if (strength < MATH_EPSILON)
			((btGeneric6DofSpringConstraint*)constraint_)->enableSpring(property, false);
		else
		{
			((btGeneric6DofSpringConstraint*)constraint_)->enableSpring(property, true);
			((btGeneric6DofSpringConstraint*)constraint_)->setStiffness(property, strength);
			((btGeneric6DofSpringConstraint*)constraint_)->setEquilibriumPoint(property);
		}
	}

	void PhysicsSpringConstraint::SetDamping(SpringProperty property, float damping)
	{
		SC_ASSERT(constraint_);
		((btGeneric6DofSpringConstraint*)constraint_)->setDamping(property, damping);
		((btGeneric6DofSpringConstraint*)constraint_)->setEquilibriumPoint(property);
	}

} // namespace scythe