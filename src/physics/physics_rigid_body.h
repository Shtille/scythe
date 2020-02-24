#ifndef __SCYTHE_PHYSICS_RIGID_BODY_H__
#define __SCYTHE_PHYSICS_RIGID_BODY_H__

#include "physics_constraint.h"
#include "physics_collision_object.h"

#include "math/transform.h"

#include <vector>

class btRigidBody;

namespace scythe {

	/**
	 * Defines a class for physics rigid bodies.
	 *
	 * A rigid body can receive forces and torque to make your objects react to other collision
	 * objects around it.
	 */
	class PhysicsRigidBody : public PhysicsCollisionObject, public Transform::Listener
	{
		friend class Node;
		friend class PhysicsCharacter;
		friend class PhysicsVehicle;
		friend class PhysicsVehicleWheel;
		friend class PhysicsConstraint;
		friend class PhysicsController;
		friend class PhysicsFixedConstraint;
		friend class PhysicsGenericConstraint;
		friend class PhysicsHingeConstraint;
		friend class PhysicsSocketConstraint;
		friend class PhysicsSpringConstraint;

	public:
		/**
		 * Rigid body construction parameters.
		 */
		struct Parameters
		{
			/**
			 * The mass of the rigid body, in kilograms.
			 */
			float mass;

			/**
			 * The friction of the rigid body (non-zero values give best simulation results).
			 */
			float friction;

			/**
			 * The restitution of the rigid body (this controls the bounciness of
			 * the rigid body; use zero for best simulation results).
			 */
			float restitution;

			/**
			 * The percentage of linear velocity lost per second (between 0.0 and 1.0).
			 */
			float linearDamping;

			/**
			 * The percentage of angular velocity lost per second (between 0.0 and 1.0).
			 */
			float angularDamping;

			/**
			 * Whether the rigid body is kinematic.
			 */
			bool kinematic;

			/**
			 * The anisotropic friction term for the rigid body.
			 */
			Vector3 anisotropicFriction;

			/**
			 * Linear factor for the rigid body. x, y, z coordinates correspond to world 
			 * space motion along these axes. Use 1.0 to allow or 0.0 to disallow motion 
			 * along certain axis.
			 */
			Vector3 linearFactor;

			/**
			 * Angular factor for the rigid body. x, y, z coordinates correspond to world 
			 * space rotation along these axes. Use 1.0 to allow or 0.0 to disallow rotation
			 * along certain axis.
			 */
			Vector3 angularFactor;

			/**
			 * Constructor.
			 */
			Parameters();

			/**
			 * Constructor.
			 */
			Parameters(float mass, float friction = 0.5f, float restitution = 0.0f,
				float linearDamping = 0.0f, float angularDamping = 0.0f, bool kinematic = false,
				const Vector3& anisotropicFriction = Vector3::One(),
				const Vector3& linearFactor = Vector3::One(), 
				const Vector3& angularFactor = Vector3::One());
		};

		/**
		 * @see PhysicsCollisionObject::type
		 */
		PhysicsCollisionObject::Type type() const override;

		/**
		 * Gets the rigid body's mass.
		 *
		 * @return The mass.
		 */
		inline float GetMass() const;

		/**
		 * Gets the rigid body's friction.
		 * 
		 * @return The friction.
		 */
		inline float GetFriction() const;

		/**
		 * Sets the rigid body's friction.
		 * 
		 * @param friction The friction.
		 */
		inline void SetFriction(float friction);

		/**
		 * Sets the rigid body's rolling friction.
		 * 
		 * @param friction The friction.
		 */
		inline void SetRollingFriction(float friction);

		/**
		 * Sets the rigid body's spinning friction.
		 * 
		 * @param friction The friction.
		 */
		inline void SetSpinningFriction(float friction);

		/**
		 * Gets the rigid body's restitution.
		 *
		 * @return The restitution.
		 */
		inline float GetRestitution() const;

		/**
		 * Sets the rigid body's restitution (or bounciness).
		 * 
		 * @param restitution The restitution.
		 */
		inline void SetRestitution(float restitution);

		/**
		 * Gets the rigid body's linear damping.
		 *
		 * @return The linear damping.
		 */
		inline float GetLinearDamping() const;

		/**
		 * Gets the rigid body's angular damping.
		 *
		 * @return The angular damping.
		 */
		inline float GetAngularDamping() const;

		/**
		 * Sets the rigid body's linear and angular damping.
		 * 
		 * @param linearDamping The linear damping; between 0.0 (minimum) and 1.0 (maximum).
		 * @param angularDamping The angular damping; between 0.0 (minimum) and 1.0 (maximum).
		 */
		inline void SetDamping(float linearDamping, float angularDamping);

		/**
		 * Gets the rigid body's contact processing threshold.
		 * 
		 * @return The contact processing threshold.
		 */
		inline float GetContactProcessingThreshold() const;

		/**
		 * Sets the rigid body's contact processing threshold.
		 * 
		 * @param threshold The contact processing threshold.
		 */
		inline void SetContactProcessingThreshold(float threshold);

		/**
		 * Gets the rigid body's contact stiffness.
		 * 
		 * @return The contact stiffness.
		 */
		inline float GetContactStiffness() const;

		/**
		 * Gets the rigid body's contact damping.
		 * 
		 * @return The contact damping.
		 */
		inline float GetContactDamping() const;

		/**
		 * Sets the rigid body's contact stiffness and damping.
		 * 
		 * @param stiffness The contact stiffness.
		 * @param damping The contact damping.
		 */
		inline void SetContactStiffnessAndDamping(float stiffness, float damping);

		/**
		 * Gets the rigid body's linear velocity.
		 * 
		 * @return The linear velocity.
		 */
		inline Vector3 GetLinearVelocity() const;

		/**
		 * Sets the rigid body's linear velocity.
		 * 
		 * @param velocity The linear velocity.
		 */
		inline void SetLinearVelocity(const Vector3& velocity);

		/**
		 * Sets the rigid body's linear velocity.
		 * 
		 * @param x The x coordinate of the linear velocity vector.
		 * @param y The y coordinate of the linear velocity vector.
		 * @param z The z coordinate of the linear velocity vector.
		 */
		inline void SetLinearVelocity(float x, float y, float z);

		/**
		 * Gets the rigid body's angular velocity.
		 * 
		 * @return The angular velocity.
		 */
		inline Vector3 GetAngularVelocity() const;

		/**
		 * Sets the rigid body's angular velocity.
		 * 
		 * @param velocity The angular velocity.
		 */
		inline void SetAngularVelocity(const Vector3& velocity);

		/**
		 * Sets the rigid body's angular velocity.
		 * 
		 * @param x The x coordinate of the angular velocity vector.
		 * @param y The y coordinate of the angular velocity vector.
		 * @param z The z coordinate of the angular velocity vector.
		 */
		inline void SetAngularVelocity(float x, float y, float z);

		/**
		* Adds speed limit.
		*
		* @param info The speed limit information.
		*/
		void AddSpeedLimit(const SpeedLimitInfo& info);

		/**
		* Removes speed limit.
		*/
		void RemoveSpeedLimit();

		/**
		 * Gets the rigid body's anisotropic friction.
		 * 
		 * @return The anisotropic friction.
		 */
		inline Vector3 GetAnisotropicFriction() const;

		/**
		 * Sets the rigid body's anisotropic friction.
		 * 
		 * @param friction The anisotropic friction.
		 */
		inline void SetAnisotropicFriction(const Vector3& friction);

		/**
		 * Sets the rigid body's anisotropic friction.
		 * 
		 * @param x The x coordinate of the anisotropic friction.
		 * @param y The y coordinate of the anisotropic friction.
		 * @param z The z coordinate of the anisotropic friction.
		 */
		inline void SetAnisotropicFriction(float x, float y, float z);

		/**
		 * Gets the gravity that affects the rigid body (this can
		 * be different from the global gravity; @see setGravity(Vector3)).
		 * 
		 * @return The gravity.
		 */
		inline Vector3 GetGravity() const;

		/**
		 * Sets the rigid body's gravity (this overrides the global gravity for this rigid body).
		 * 
		 * @param gravity The gravity.
		 */
		inline void SetGravity(const Vector3& gravity);

		/**
		 * Sets the rigid body's gravity (this overrides the global gravity for this rigid body).
		 * 
		 * @param x The x coordinate of the gravity vector.
		 * @param y The y coordinate of the gravity vector.
		 * @param z The z coordinate of the gravity vector.
		 */
		inline void SetGravity(float x, float y, float z);

		/**
		 * Gets the rigid body's angular factor.
		 * 
		 * @return The angular factor.
		 */
		inline Vector3 GetAngularFactor() const;

		/**
		 * Sets the rigid body's angular factor.  x, y, z coordinates correspond to world 
		 * space rotation along these axes. Use 1.0 to allow or 0.0 to disallow rotation 
		 * along certain axis.
		 * 
		 * @param angularFactor angular factor vector
		 */
		inline void SetAngularFactor(const Vector3& angularFactor);

		/**
		 * Sets the rigid body's angular factor.  x, y, z coordinates correspond to world 
		 * space rotation along these axes. Use 1.0 to allow or 0.0 to disallow rotation 
		 * along certain axis.
		 * 
		 * @param x The x coordinate of the angular factor vector.
		 * @param y The y coordinate of the angular factor vector.
		 * @param z The z coordinate of the angular factor vector.
		 */
		inline void SetAngularFactor(float x, float y, float z);

		/**
		 * Gets the rigid body's linear factor.
		 * 
		 * @return The linear factor.
		 */
		inline Vector3 GetLinearFactor() const;

		/**
		 * Sets the rigid body's linear factor.  x, y, z coordinates correspond to world 
		 * space motion along these axes. Use 1.0 to allow or 0.0 to disallow motion 
		 * along certain axis.
		 * 
		 * @param linearFactor linear factor vector
		 */
		inline void SetLinearFactor(const Vector3& linearFactor);

		/**
		 * Sets the rigid body's linear factor.  x, y, z coordinates correspond to world 
		 * space motion along these axes. Use 1.0 to allow or 0.0 to disallow motion 
		 * along certain axis.
		 * 
		 * @param x The x coordinate of the linear factor vector.
		 * @param y The y coordinate of the linear factor vector.
		 * @param z The z coordinate of the linear factor vector.
		 */
		inline void SetLinearFactor(float x, float y, float z);

		/**
		 * Disables rigid body deactivation.
		 */
		inline void DisableDeactivation();

		/**
		 * Sets whether the rigid body is a kinematic rigid body or not.
		 * 
		 * @param kinematic Whether the rigid body is kinematic or not.
		 */
		void SetKinematic(bool kinematic);

		/**
		 * Sets whether the rigid body is enabled or disabled in the physics world.
		 *
		 * @param enable true enables the collision object, false disables it.
		 */
		void SetEnabled(bool enable);

		/**
		 * Gets whether the rigid body is a static rigid body or not.
		 *
		 * @return Whether the rigid body is static.
		 */
		inline bool IsStatic() const;

		/**
		 * Applies the given force to the rigid body (optionally, from the given relative position).
		 * Note that the total force applied depends on the duration of the next frame.
		 * If you want to apply an "impulse" irrespective of the frame duration, consider using applyImpulse.
		 * 
		 * @param force The force to be applied.
		 * @param relativePosition The relative position from which to apply the force.
		 */
		void ApplyForce(const Vector3& force, const Vector3* relativePosition = NULL);

		/**
		 * Applies the given force impulse to the rigid body (optionally, from the given relative position).
		 * 
		 * @param impulse The force impulse to be applied.
		 * @param relativePosition The relative position from which to apply the force.
		 */
		void ApplyImpulse(const Vector3& impulse, const Vector3* relativePosition = NULL);

		/**
		 * Applies the given torque to the rigid body.
		 * 
		 * @param torque The torque to be applied.
		 */
		void ApplyTorque(const Vector3& torque);

		/**
		 * Applies the given torque impulse to the rigid body.
		 * 
		 * @param torque The torque impulse to be applied.
		 */
		void ApplyTorqueImpulse(const Vector3& torque);

	protected:

		/**
		 * @see PhysicsCollisionObject::getCollisionObject
		 */
		btCollisionObject* GetCollisionObject() const override;

	private:
		/**
		 * Creates a rigid body.
		 * 
		 * @param node The node to create a rigid body for; note that the node must have
		 *      a model attached to it prior to creating a rigid body for it.
		 * @param shape The rigid body shape construction information.
		 * @param parameters The rigid body construction parameters.
		 * @param group Group identifier
		 * @param mask Bitmask field for filtering collisions with this object.
		 */
		PhysicsRigidBody(Node* node, const PhysicsCollisionShape::Definition& shape, const Parameters& parameters, int group = PHYSICS_COLLISION_GROUP_DEFAULT, int mask = PHYSICS_COLLISION_MASK_DEFAULT);

		/**
		 * Destructor.
		 */
		~PhysicsRigidBody();

		/**
		 * Private copy constructor to disallow copying.
		 */
		PhysicsRigidBody(const PhysicsRigidBody& body);

		// Adds a constraint to this rigid body.
		void AddConstraint(PhysicsConstraint* constraint);

		// Removes a constraint from this rigid body (used by the constraint destructor).
		void RemoveConstraint(PhysicsConstraint* constraint);

		// Whether or not the rigid body supports constraints fully.
		bool SupportsConstraints();

		// Used for implementing getHeight() when the heightfield has a transform that can change.
		void TransformChanged(Transform * transform, long cookie) override;

		// Clamps linear velocity.
		void ClampLinearVelocity(float max_speed) override;

		// Clamps angular velocity.
		void ClampAngularVelocity(float max_speed) override;

		btRigidBody * body_;
		float mass_;
		std::vector<PhysicsConstraint*>* constraints_;
		bool in_destructor_;
	};

} // namespace scythe

#include "physics_rigid_body.inl"

#endif