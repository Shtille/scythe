#ifndef __SCYTHE_PHYSICS_CONSTRAINT_H__
#define __SCYTHE_PHYSICS_CONSTRAINT_H__

#include "math/vector.h"
#include "math/quaternion.h"

#include "bullet_include.h"

// Forward Bullet declarations
class btTypedConstraint;

namespace scythe {

	// Forward declarations
	class Node;
	class PhysicsRigidBody;

	/**
	* Defines the base class for physics constraints.
	*/
	class PhysicsConstraint {

		friend class PhysicsRigidBody;

	public:
		/**
		 * Gets the impulse needed to break the constraint.
		 * 
		 * @return The impulse needed to break the constraint.
		 */
		float GetBreakingImpulse() const;

		/**
		 * Sets the impulse needed to break the constraint
		 * (if an impulse greater than or equal to the given
		 * value is applied to the constraint, the constraint
		 * will be broken).
		 * 
		 * @param impulse The impulse needed to break the constraint.
		 */
		inline void SetBreakingImpulse(float impulse);

		/**
		 * Gets whether the constraint is enabled or not.
		 * 
		 * @return Whether the constraint is enabled or not.
		 */
		inline bool IsEnabled() const;

		/**
		 * Sets whether the constraint is enabled or not.
		 * 
		 * @param enabled Whether the constraint is enabled or not.
		 */
		inline void SetEnabled(bool enabled);

		/**
		 * Calculates the midpoint between the given nodes' centers of mass.
		 * 
		 * @param a The first node.
		 * @param b The second node.
		 */
		static Vector3 CenterOfMassMidpoint(const Node* a, const Node* b);

		/**
		 * Calculates the rotation offset to the given point in the given node's local space.
		 * 
		 * @param node The node to calculate a rotation offset for.
		 * @param point The point to calculate the rotation offset to.
		 */
		static Quaternion GetRotationOffset(const Node* node, const Vector3& point);

		/**
		 * Calculates the translation offset to the given point in the given node's local space.
		 * 
		 * @param node The node to calculate a translation offset for.
		 * @param point The point to calculate the translation offset to.
		 */
		static Vector3 GetTranslationOffset(const Node* node, const Vector3& point);

	protected:

		/**
		 * Constructor.
		 */
		PhysicsConstraint(PhysicsRigidBody* a, PhysicsRigidBody* b);

		/**
		 * Destructor.
		 */
		virtual ~PhysicsConstraint();

		/**
		 * Calculates the transform to be used as the offset (i.e. "frame in" 
		 * parameter in Bullet terms) to the given constraint origin.
		 */
		static btTransform GetTransformOffset(const Node* node, const Vector3& origin);
		
		/**
		 * Calculates the center of mass in world space of the given node.
		 */
		static Vector3 GetWorldCenterOfMass(const Node* node);

		/**
		 * Offsets the given vector by the given node's center of mass.
		 */
		static Vector3 OffsetByCenterOfMass(const Node* node, const Vector3& v);

		/**
		 * Pointer to the one rigid body bound by this constraint.
		 */
		PhysicsRigidBody * a_;
		
		/**
		 * Pointer to the other rigid body bound by this constraint.
		 */
		PhysicsRigidBody * b_;
		
		/**
		 * Pointer to the Bullet constraint.
		 */
		btTypedConstraint * constraint_;
	};

} // namespace scythe

#include "physics_constraint.inl"

#endif