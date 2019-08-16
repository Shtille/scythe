#include "physics_contraint.h"

#include "common/sc_assert.h"
#include "common/sc_delete.h"

namespace scythe {

	PhysicsConstraint::PhysicsConstraint(PhysicsRigidBody* a, PhysicsRigidBody* b)
	: a_(a)
	, b_(b)
	, constraint_(nullptr)
	{
	}

	PhysicsConstraint::~PhysicsConstraint()
	{
		// Remove the physics rigid bodies' references to this constraint.
		if (a_)
			a_->RemoveConstraint(this);
		if (b_)
			b_->RemoveConstraint(this);

		// Remove the constraint from the physics world and delete the Bullet object.
		PhysicsController * controller = PhysicsController::GetInstance();
		SC_ASSERT(controller);
		controller->RemoveConstraint(this);
		SC_SAFE_DELETE(constraint_);
	}

	Vector3 PhysicsConstraint::CenterOfMassMidpoint(const Node* a, const Node* b)
	{
		SC_ASSERT(a);
		SC_ASSERT(b);

		Vector3 tA, tB;
		a->GetWorldMatrix().GetTranslation(&tA);
		b->GetWorldMatrix().GetTranslation(&tB);

		tA = GetWorldCenterOfMass(a);
		tB = GetWorldCenterOfMass(b);
		
		Vector3 d(tA, tB);
		d.Scale(0.5f);
		Vector3 c(tA);
		c.Add(d);

		return c;
	}

	Quaternion PhysicsConstraint::GetRotationOffset(const Node* node, const Vector3& point)
	{
		SC_ASSERT(node);

		// Create a translation matrix that translates to the given origin.
		Matrix m;
		Matrix::CreateTranslation(point, &m);

		// Calculate the rotation offset to the rigid body by transforming 
		// the translation matrix above into the rigid body's local space 
		// (multiply by the inverse world matrix) and extracting the rotation.
		Matrix mi;
		node->GetWorldMatrix().Invert(&mi);
		mi.Multiply(m);
		
		Quaternion r;
		mi.GetRotation(&r);

		return r;
	}

	Vector3 PhysicsConstraint::GetTranslationOffset(const Node* node, const Vector3& point)
	{
		SC_ASSERT(node);

		// Create a translation matrix that translates to the given origin.
		Matrix m;
		Matrix::CreateTranslation(point, &m);

		// Calculate the translation offset to the rigid body by transforming 
		// the translation matrix above into the rigid body's local space 
		// (multiply by the inverse world matrix) and extracting the translation.
		Matrix mi;
		node->GetWorldMatrix().Invert(&mi);
		mi.Multiply(m);
		
		Vector3 t;
		mi.GetTranslation(&t);

		Vector3 s;
		node->GetWorldMatrix().GetScale(&s);

		t.x *= s.x;
		t.y *= s.y;
		t.z *= s.z;
		
		t = OffsetByCenterOfMass(node, t);

		return t;
	}

	btTransform PhysicsConstraint::GetTransformOffset(const Node* node, const Vector3& origin)
	{
		SC_ASSERT(node);

		// Create a translation matrix that translates to the given origin.
		Matrix m;
		Matrix::CreateTranslation(origin, &m);

		// Calculate the translation and rotation offset to the rigid body
		// by transforming the translation matrix above into the rigid body's
		// local space (multiply by the inverse world matrix and extract components).
		Matrix mi;
		node->GetWorldMatrix().Invert(&mi);
		mi.Multiply(m);

		Quaternion r;
		mi.GetRotation(&r);
		
		Vector3 t;
		mi.GetTranslation(&t);

		Vector3 s;
		node->GetWorldMatrix().GetScale(&s);

		t.x *= s.x;
		t.y *= s.y;
		t.z *= s.z;
		
		t = OffsetByCenterOfMass(node, t);

		return btTransform(BQ(r), BV(t));
	}

	Vector3 PhysicsConstraint::GetWorldCenterOfMass(const Node* node)
	{
		SC_ASSERT(node);

		const BoundingSphere& sphere = node->GetBoundingSphere();
		if (!(sphere.center.IsZero() && sphere.radius == 0))
		{
			// The world-space center of mass is the sphere's center.
			return sphere.center;
		}

		// Warn the user that the node has no bounding volume.
		//GP_WARN("Node %s' has no bounding volume - center of mass is defaulting to local coordinate origin.", node->getId());

		Vector3 center;
		node->GetWorldMatrix().TransformPoint(&center);
		return center;
	}

	Vector3 PhysicsConstraint::OffsetByCenterOfMass(const Node* node, const Vector3& v)
	{
		SC_ASSERT(node && node->GetCollisionObject() && node->GetCollisionObject()->motion_state_);
		btVector3 centerOfMassOffset = node->GetCollisionObject()->motion_state_->center_of_mass_offset_.getOrigin();
		return Vector3(v.x + centerOfMassOffset.x(), v.y + centerOfMassOffset.y(), v.z + centerOfMassOffset.z());
	}

} // namespace scythe

#endif