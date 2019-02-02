#ifndef __SCYTHE_PHYSICS_FIXED_CONSTRAINT_H__
#define __SCYTHE_PHYSICS_FIXED_CONSTRAINT_H__

#include "physics_generic_constraint.h"

namespace scythe {

	/**
	 * Defines a constraint where two rigid bodies 
	 * (or one rigid body and the world) are bound together.
	 *
	 * This is similar in concept to parenting one node to another,
	 * but can be used in specific situations for a more appropriate effect
	 * Ex. for implementing sticky projectiles, etc.
	 */
	class PhysicsFixedConstraint : public PhysicsGenericConstraint
	{
		friend class PhysicsController;

	protected:

		/**
		 * @see PhysicsGenericConstraint
		 */
		PhysicsFixedConstraint(PhysicsRigidBody* a, PhysicsRigidBody* b);

		/**
		 * Destructor.
		 */
		~PhysicsFixedConstraint();

		// Note: We make these functions protected to prevent usage
		// (these are public in the base class, PhysicsGenericConstraint).

		/**
		 * Protected to prevent usage.
		 */
		inline void SetAngularLowerLimit(const Vector3& limit);
		
		/**
		 * Protected to prevent usage.
		 */
		inline void SetAngularUpperLimit(const Vector3& limit);
		
		/**
		 * Protected to prevent usage.
		 */
		inline void SetLinearLowerLimit(const Vector3& limit);
		
		/**
		 * Protected to prevent usage.
		 */
		inline void SetLinearUpperLimit(const Vector3& limit);
	};

} // namespace scythe

#include "physics_fixed_constraint.inl"

#endif