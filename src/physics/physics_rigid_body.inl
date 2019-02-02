#include "physics_rigid_body.h"

#include "common/sc_assert.h"

namespace scythe {

	inline float PhysicsRigidBody::GetMass() const
	{
		return mass_;
	}

	inline float PhysicsRigidBody::GetFriction() const
	{
		SC_ASSERT(body_);
		return body_->getFriction();
	}

	inline void PhysicsRigidBody::SetFriction(float friction)
	{
		SC_ASSERT(body_);
		body_->setFriction(friction);
	}

	inline float PhysicsRigidBody::GetRestitution() const
	{
		SC_ASSERT(body_);
		return body_->getRestitution();
	}

	inline void PhysicsRigidBody::SetRestitution(float restitution)
	{
		SC_ASSERT(body_);
		body_->setRestitution(restitution);
	}

	inline float PhysicsRigidBody::GetLinearDamping() const
	{
		SC_ASSERT(body_);
		return body_->getLinearDamping();
	}

	inline float PhysicsRigidBody::GetAngularDamping() const
	{
		SC_ASSERT(body_);
		return body_->getAngularDamping();
	}

	inline void PhysicsRigidBody::SetDamping(float linearDamping, float angularDamping)
	{
		SC_ASSERT(body_);
		body_->setDamping(linearDamping, angularDamping);
	}

	inline Vector3 PhysicsRigidBody::GetLinearVelocity() const
	{
		SC_ASSERT(body_);
		const btVector3& v = body_->getLinearVelocity();
		return Vector3(v.x(), v.y(), v.z());
	}

	inline void PhysicsRigidBody::SetLinearVelocity(const Vector3& velocity)
	{
		SC_ASSERT(body_);
		body_->setLinearVelocity(BV(velocity));
	}

	inline void PhysicsRigidBody::SetLinearVelocity(float x, float y, float z)
	{
		SC_ASSERT(body_);
		body_->setLinearVelocity(btVector3(x, y, z));
	}

	inline Vector3 PhysicsRigidBody::GetAngularVelocity() const
	{
		SC_ASSERT(body_);
		const btVector3& v = body_->getAngularVelocity();
		return Vector3(v.x(), v.y(), v.z());
	}

	inline void PhysicsRigidBody::SetAngularVelocity(const Vector3& velocity)
	{
		SC_ASSERT(body_);
		body_->setAngularVelocity(BV(velocity));
	}

	inline void PhysicsRigidBody::SetAngularVelocity(float x, float y, float z)
	{
		SC_ASSERT(body_);
		body_->setAngularVelocity(btVector3(x, y, z));
	}

	inline Vector3 PhysicsRigidBody::GetAnisotropicFriction() const
	{
		SC_ASSERT(body_);
		const btVector3& af = body_->getAnisotropicFriction();
		return Vector3(af.x(), af.y(), af.z());
	}

	inline void PhysicsRigidBody::SetAnisotropicFriction(const Vector3& friction)
	{
		SC_ASSERT(body_);
		body_->setAnisotropicFriction(BV(friction));
	}

	inline void PhysicsRigidBody::SetAnisotropicFriction(float x, float y, float z)
	{
		SC_ASSERT(body_);
		body_->setAnisotropicFriction(btVector3(x, y, z));
	}

	inline Vector3 PhysicsRigidBody::GetGravity() const
	{
		SC_ASSERT(body_);
		const btVector3& g = body_->getGravity();
		return Vector3(g.x(), g.y(), g.z());
	}

	inline void PhysicsRigidBody::SetGravity(const Vector3& gravity)
	{
		SC_ASSERT(body_);
		body_->setGravity(BV(gravity));
	}

	inline void PhysicsRigidBody::SetGravity(float x, float y, float z)
	{
		SC_ASSERT(body_);
		body_->setGravity(btVector3(x, y, z));
	}

	inline Vector3 PhysicsRigidBody::GetAngularFactor() const
	{
		SC_ASSERT(body_);
		const btVector3& f = body_->getAngularFactor();
		return Vector3(f.x(), f.y(), f.z());
	}

	inline void PhysicsRigidBody::SetAngularFactor(const Vector3& angularFactor)
	{
		SC_ASSERT(body_);
		body_->setAngularFactor(BV(angularFactor));
	}

	inline void PhysicsRigidBody::SetAngularFactor(float x, float y, float z)
	{
		SC_ASSERT(body_);
		body_->setAngularFactor(btVector3(x, y, z));
	}

	inline Vector3 PhysicsRigidBody::GetLinearFactor() const
	{
		SC_ASSERT(body_);
		const btVector3& f = body_->getLinearFactor();
		return Vector3(f.x(), f.y(), f.z());
	}

	inline void PhysicsRigidBody::SetLinearFactor(const Vector3& angularFactor)
	{
		SC_ASSERT(body_);
		body_->setLinearFactor(BV(angularFactor));
	}

	inline void PhysicsRigidBody::SetLinearFactor(float x, float y, float z)
	{
		SC_ASSERT(body_);
		body_->setLinearFactor(btVector3(x, y, z));
	}

	inline bool PhysicsRigidBody::IsStatic() const
	{
		SC_ASSERT(body_);
		return body_->isStaticObject();
	}

} // namespace scythe