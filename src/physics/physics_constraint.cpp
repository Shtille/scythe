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
			a_->removeConstraint(this);
		if (b_)
			b_->removeConstraint(this);

		// Remove the constraint from the physics world and delete the Bullet object.
		PhysicsController * controller = PhysicsController::GetInstance();
		SC_ASSERT(controller);
		controller->removeConstraint(this);
		SC_SAFE_DELETE(constraint_);
	}

	Vector3 PhysicsConstraint::centerOfMassMidpoint(const Node* a, const Node* b)
	{
		SC_ASSERT(a);
		SC_ASSERT(b);

		Vector3 tA, tB;
		a->getWorldMatrix().getTranslation(&tA);
		b->getWorldMatrix().getTranslation(&tB);

		tA = getWorldCenterOfMass(a);
		tB = getWorldCenterOfMass(b);
		
		Vector3 d(tA, tB);
		d.scale(0.5f);
		Vector3 c(tA);
		c.add(d);

		return c;
	}

	Quaternion PhysicsConstraint::getRotationOffset(const Node* node, const Vector3& point)
	{
		SC_ASSERT(node);

		// Create a translation matrix that translates to the given origin.
		Matrix m;
		Matrix::createTranslation(point, &m);

		// Calculate the rotation offset to the rigid body by transforming 
		// the translation matrix above into the rigid body's local space 
		// (multiply by the inverse world matrix) and extracting the rotation.
		Matrix mi;
		node->getWorldMatrix().invert(&mi);
		mi.multiply(m);
		
		Quaternion r;
		mi.getRotation(&r);

		return r;
	}

	Vector3 PhysicsConstraint::getTranslationOffset(const Node* node, const Vector3& point)
	{
		SC_ASSERT(node);

		// Create a translation matrix that translates to the given origin.
		Matrix m;
		Matrix::createTranslation(point, &m);

		// Calculate the translation offset to the rigid body by transforming 
		// the translation matrix above into the rigid body's local space 
		// (multiply by the inverse world matrix) and extracting the translation.
		Matrix mi;
		node->getWorldMatrix().invert(&mi);
		mi.multiply(m);
		
		Vector3 t;
		mi.getTranslation(&t);

		Vector3 s;
		node->getWorldMatrix().getScale(&s);

		t.x *= s.x;
		t.y *= s.y;
		t.z *= s.z;
		
		t = offsetByCenterOfMass(node, t);

		return t;
	}

	btTransform PhysicsConstraint::getTransformOffset(const Node* node, const Vector3& origin)
	{
		SC_ASSERT(node);

		// Create a translation matrix that translates to the given origin.
		Matrix m;
		Matrix::createTranslation(origin, &m);

		// Calculate the translation and rotation offset to the rigid body
		// by transforming the translation matrix above into the rigid body's
		// local space (multiply by the inverse world matrix and extract components).
		Matrix mi;
		node->getWorldMatrix().invert(&mi);
		mi.multiply(m);

		Quaternion r;
		mi.getRotation(&r);
		
		Vector3 t;
		mi.getTranslation(&t);

		Vector3 s;
		node->getWorldMatrix().getScale(&s);

		t.x *= s.x;
		t.y *= s.y;
		t.z *= s.z;
		
		t = offsetByCenterOfMass(node, t);

		return btTransform(BQ(r), BV(t));
	}

	Vector3 PhysicsConstraint::getWorldCenterOfMass(const Node* node)
	{
		SC_ASSERT(node);

		const BoundingSphere& sphere = node->getBoundingSphere();
		if (!(sphere.center.isZero() && sphere.radius == 0))
		{
			// The world-space center of mass is the sphere's center.
			return sphere.center;
		}

		// Warn the user that the node has no bounding volume.
		//GP_WARN("Node %s' has no bounding volume - center of mass is defaulting to local coordinate origin.", node->getId());

		Vector3 center;
		node->getWorldMatrix().transformPoint(&center);
		return center;
	}

	Vector3 PhysicsConstraint::offsetByCenterOfMass(const Node* node, const Vector3& v)
	{
		SC_ASSERT(node && node->getCollisionObject() && node->getCollisionObject()->motion_state_);
		btVector3 centerOfMassOffset = node->getCollisionObject()->motion_state_->_centerOfMassOffset.getOrigin();
		return Vector3(v.x + centerOfMassOffset.x(), v.y + centerOfMassOffset.y(), v.z + centerOfMassOffset.z());
	}

} // namespace scythe

#endif