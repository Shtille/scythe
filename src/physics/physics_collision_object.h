#ifndef __SCYTHE_PHYSICS_COLLISION_OBJECT_H__
#define __SCYTHE_PHYSICS_COLLISION_OBJECT_H__

#include "physics_collision_shape.h"

#include "bullet_include.h"

#define PHYSICS_COLLISION_GROUP_DEFAULT btBroadphaseProxy::DefaultFilter
#define PHYSICS_COLLISION_MASK_DEFAULT btBroadphaseProxy::AllFilter

namespace scythe {

	class Node;

	/**
	 * Defines the base class for all physics objects that support collision events.
	 */
	class PhysicsCollisionObject {

		friend class PhysicsController;
		friend class PhysicsConstraint;
		friend class PhysicsRigidBody;
		friend class PhysicsGhostObject;

	public:
		/**
		 * Represents the different types of collision objects.
		 */
		enum Type {
			kNone,			//!< No collision object.
			kRigidBody,		//!< PhysicsRigidBody type.
			kCharacter,		//!< PhysicsCharacter type.
			kGhostObject,	//!< PhysicsGhostObject type.
			kVehicle,		//!< PhysicsVehicle type.
			kVehicleWheel	//!< PhysicsVehicleWheel type.
		};

		/** 
		 * Defines a pair of rigid bodies that collided (or may collide).
		 */
		class CollisionPair {
		public:

			/**
			 * Constructor.
			 */
			CollisionPair(PhysicsCollisionObject* objectA, PhysicsCollisionObject* objectB);

			/**
			 * Less than operator (needed for use as a key in map).
			 * 
			 * @param collisionPair The collision pair to compare.
			 * @return True if this pair is "less than" the given pair; false otherwise.
			 */
			bool operator < (const CollisionPair& collisionPair) const;

			/**
			 * The first object in the collision.
			 */
			PhysicsCollisionObject* objectA;

			/**
			 * The second object in the collision.
			 */
			PhysicsCollisionObject* objectB;
		};

		/**
		 * Collision listener interface.
		 */
		class CollisionListener {

			friend class PhysicsCollisionObject;
			friend class PhysicsController;

		public:

			/**
			 * The type of collision event.
			 */
			enum EventType
			{
				/**
				 * Event fired when the two rigid bodies start colliding.
				 */
				kColliding,

				/**
				 * Event fired when the two rigid bodies no longer collide.
				 */
				kNotColliding
			};

			/**
			 * Virtual destructor.
			 */
			virtual ~CollisionListener() { }

			/**
			 * Called when a collision occurs between two objects in the physics world.
			 * 
			 * NOTE: You are not permitted to disable physics objects from within this callback. Disabling physics on a collision object
			 *  removes the object from the physics world. This is not permitted during the PhysicsController::update.
			 *
			 * @param type The type of collision event.
			 * @param collisionPair The two collision objects involved in the collision.
			 * @param contactPointA The contact point with the first object (in world space).
			 * @param contactPointB The contact point with the second object (in world space).
			 */
			virtual void CollisionEvent(PhysicsCollisionObject::CollisionListener::EventType type,
										const PhysicsCollisionObject::CollisionPair& collisionPair,
										const Vector3& contactPoint  = Vector3::Zero(),
										const Vector3& contactNormal = Vector3::UnitY()) = 0;
		};

		/**
		* Structure that holds speed limit information.
		*/
		struct SpeedLimitInfo
		{
			/**
			* Maximum linear velocity.
			*/
			float max_linear_velocity;

			/**
			* Maximum angular velocity.
			*/
			float max_angular_velocity;

			/**
			* Whether clamp linear velocity.
			*/
			bool clamp_linear_velocity;

			/**
			* Whether clamp angular velocity.
			*/
			bool clamp_angular_velocity;
		};

		/**
		 * Virtual destructor.
		 */
		virtual ~PhysicsCollisionObject();

		/**
		 * Returns the type of the collision object.
		 */
		virtual PhysicsCollisionObject::Type type() const = 0;

		/**
		 * Returns the type of the shape for this collision object.
		 */
		PhysicsCollisionShape::Type GetShapeType() const;

		/**
		 * Returns the node associated with this collision object.
		 */
		Node* GetNode() const;

		/**
		 * Returns the collision shape.
		 *
		 * @return The collision shape.
		 */
		PhysicsCollisionShape * GetCollisionShape() const;

		/**
		 * Returns whether this collision object is kinematic.
		 *
		 * A kinematic collision object is an object that is not simulated by
		 * the physics system and instead has its transform driven manually.
		 *
		 * @return true if the collision object is kinematic.
		 */
		bool IsKinematic() const;

		/**
		 * Returns whether this collision object is static.
		 *
		 * A static collision object is not simulated by the physics system and cannot be
		 * transformed once created.
		 *
		 * @return true if the collision object is static.
		 */
		bool IsStatic() const;

		/**
		 * Returns whether this collision object is dynamic.
		 *
		 * A dynamic collision object is simulated entirely by the physics system,
		 * such as with dynamic rigid bodies. 
		 *
		 * @return true if the collision object is dynamic.
		 */
		bool IsDynamic() const;

		/**
		 * Check if the collision object is enabled.
		 *
		 * @return true if the collision object is enabled.
		 */
		bool IsEnabled() const;

		/**
		 * Sets the collision object to be enabled or disabled.
		 *
		 * @param enable true enables the collision object, false disables it.
		 */
		void SetEnabled(bool enable);

		/**
		 * Adds a collision listener for this collision object.
		 * 
		 * @param listener The listener to add.
		 * @param object Optional collision object used to filter the collision event.
		 */
		void AddCollisionListener(CollisionListener* listener, PhysicsCollisionObject* object = NULL, bool duplicate = false);

		/**
		 * Removes a collision listener.
		 *
		 * @param listener The listener to remove.
		 * @param object Optional collision object used to filter the collision event.
		 */
		void RemoveCollisionListener(CollisionListener* listener, PhysicsCollisionObject* object = NULL);

		/**
		 * Adds a collision listener for this collision object.
		 * 
		 * Note: the given script function must be global and it must match the function 
		 * signature of PhysicsCollisionObject::CollisionListener::CollisionEvent.
		 * 
		 * @param function A valid global script function to add as a listener callback.
		 * @param object Optional collision object used to filter the collision event.
		 */
		void AddCollisionListener(const char* function, PhysicsCollisionObject* object = NULL);

		/**
		 * Removes a collision listener.
		 *
		 * @param function The previously added script function to remove.
		 * @param object Optional collision object used to filter the collision event.
		 */
		void RemoveCollisionListener(const char* function, PhysicsCollisionObject* object = NULL);

		/**
		 * Checks if this collision object collides with the given object.
		 * 
		 * @param object The collision object to test for collision with.
		 * 
		 * @return true if this object collides with the specified one; false otherwise.
		 */
		bool CollidesWith(PhysicsCollisionObject* object) const;

	protected:

		/**
		 * Constructor.
		 */
		PhysicsCollisionObject(Node* node, int group = PHYSICS_COLLISION_GROUP_DEFAULT, int mask = PHYSICS_COLLISION_MASK_DEFAULT);

		/**
		 * Returns the Bullet Physics collision object.
		 *
		 * @return The Bullet collision object.
		 */
		virtual btCollisionObject * GetCollisionObject() const = 0;

		/**
		 * Clamps linear velocity.
		 *
		 * @param max_speed The maximum linear velocity magnitude.
		 */
		virtual void ClampLinearVelocity(float max_speed);

		/**
		 * Clamps angular velocity.
		 *
		 * @param max_speed The maximum angular velocity magnitude.
		 */
		virtual void ClampAngularVelocity(float max_speed);

		/**
		 * Pointer to Node contained by this collision object.
		 */ 
		Node * node_;
		
		/**
		 * The PhysicsCollisionObject's collision shape.
		 */
		PhysicsCollisionShape * collision_shape_;

		/**
		 * If the collision object is enabled or not.
		 */
		bool enabled_;

		/**
		 * The list of script listeners.
		 */
		//std::vector<ScriptListener*>* script_listeners_;

	private:

		/**
		 * Interface between Scythe and Bullet to keep object transforms synchronized properly.
		 * 
		 * @see btMotionState
		 */
		class PhysicsMotionState : public btMotionState {

			friend class PhysicsConstraint;
			
		public:
			
			/**
			 * Creates a physics motion state for a rigid body.
			 * 
			 * @param node The node that contains the transformation to be associated with the motion state.
			 * @param collisionObject The collision object that owns the motion state.
			 * @param centerOfMassOffset The translation offset to the center of mass of the rigid body.
			 */
			PhysicsMotionState(Node* node, PhysicsCollisionObject* collisionObject, const Vector3* centerOfMassOffset = NULL);
			
			/**
			 * Destructor.
			 */
			virtual ~PhysicsMotionState();
			
			/**
			 * @see btMotionState::getWorldTransform
			 */
			virtual void getWorldTransform(btTransform & transform) const;
			
			/**
			 * @see btMotionState::setWorldTransform
			 */
			virtual void setWorldTransform(const btTransform & transform);
			
			/**
			 * Updates the motion state's world transform from the GamePlay Node object's world transform.
			 */
			void UpdateTransformFromNode() const;
			
			/**
			 * Sets the center of mass offset for the associated collision shape.
			 */
			void SetCenterOfMassOffset(const Vector3& centerOfMassOffset);
			
		private:
			
			Node * node_;
			PhysicsCollisionObject * collision_object_;
			btTransform center_of_mass_offset_;
			mutable btTransform world_transform_;
		};

		/** 
		 * The PhysicsCollisionObject's motion state.
		 */
		PhysicsMotionState * motion_state_;

		/**
		 * Group identifier and the bitmask for collision filtering.
		 */
		int group_;
		int mask_;
	};

} // namespace scythe

#endif