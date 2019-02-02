#include "physics_socket_constraint.h"

#include "physics_rigid_body.h"

namespace scythe {

	PhysicsSocketConstraint::PhysicsSocketConstraint(PhysicsRigidBody* a, PhysicsRigidBody* b)
	: PhysicsConstraint(a, b)
	{
		SC_ASSERT(a && a->body_ && a->GetNode());
		if (b)
		{
			SC_ASSERT(b->body_ && b->GetNode());
			Vector3 origin = centerOfMassMidpoint(a->GetNode(), b->GetNode());
			btTransform frameInA = getTransformOffset(a->GetNode(), origin);
			btTransform frameInB = getTransformOffset(b->GetNode(), origin);

			constraint_ = new btPoint2PointConstraint(*a->body_, *b->body_, frameInA.getOrigin(), frameInB.getOrigin());
		}
		else
		{
			constraint_ = new btPoint2PointConstraint(*a->body_, btVector3(0.0f, 0.0f, 0.0f));
		}
	}

	PhysicsSocketConstraint::PhysicsSocketConstraint(PhysicsRigidBody* a, const Vector3& translationOffsetA, 
													 PhysicsRigidBody* b, const Vector3& translationOffsetB)
	: PhysicsConstraint(a, b)
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

			constraint_ = new btPoint2PointConstraint(*a->body_, *b->body_, BV(tA), BV(tB));
		}
		else
		{
			constraint_ = new btPoint2PointConstraint(*a->body_, BV(tA));
		}
	}

	PhysicsSocketConstraint::~PhysicsSocketConstraint()
	{
		// Used
	}

} // namespace scythe