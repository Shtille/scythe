#ifndef __SCYTHE_PHYSICS_GHOST_OBJECT_H__
#define __SCYTHE_PHYSICS_GHOST_OBJECT_H__

#include "physics_collision_object.h"

#include "math/transform.h"

namespace scythe {

	/**
	 * Defines a physics ghost object.
	 *
	 * It is a collision volume that does not participate in the physics
	 * simulation but can be used the test against other phyics collision objects.
	 */
	class PhysicsGhostObject : public PhysicsCollisionObject, public Transform::Listener
	{
		friend class Node;
		friend class PhysicsController;

	public:

		/**
		 * @see PhysicsCollisionObject::type
		 */
		PhysicsCollisionObject::Type type() const override;

		/**
		 * Used to synchronize the transform between Scythe and Bullet.
		 */
		void TransformChanged(Transform* transform, long cookie) override;

	protected:

		/**
		 * @see PhysicsCollisionObject::GetCollisionObject
		 */
		btCollisionObject* GetCollisionObject() const override;

	protected:

		/**
		 * Constructor.
		 * 
		 * @param node The node to attach the ghost object to.
		 * @param shape The collision shape definition for the ghost object.
		 * @param group Group identifier
		 * @param mask Bitmask field for filtering collisions with this object.
		 */
		PhysicsGhostObject(Node* node, const PhysicsCollisionShape::Definition& shape, int group = PHYSICS_COLLISION_GROUP_DEFAULT, int mask = PHYSICS_COLLISION_MASK_DEFAULT);

		/**
		 * Destructor.
		 */
		virtual ~PhysicsGhostObject();

		/**
		 * Pointer to the Bullet ghost collision object.
		 */
		btPairCachingGhostObject * ghost_object_;
	};

} // namespace scythe

#endif