#ifndef __SCYTHE_PHYSICS_CONTROLLER_H__
#define __SCYTHE_PHYSICS_CONTROLLER_H__

#include "common/singleton.h"
#include "math/vector3.h"
#include "math/ray.h"

#include <vector>
#include <map>

// Forward Bullet declarations
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDynamicsWorld;
class btGhostPairCallback;
class btCollisionObject;

namespace scythe {

	class Node;
	class Mesh;

	class PhysicsController : public ManagedSingleton<PhysicsController> {

		friend class ManagedSingleton<PhysicsController>;
		friend class PhysicsConstraint;
		friend class PhysicsRigidBody;
		friend class PhysicsCharacter;
		friend class PhysicsVehicle;
		friend class PhysicsCollisionObject;
		friend class PhysicsGhostObject;

	public:

		/**
		 * Status listener interface.
		 */
		class Listener
		{
		public:

			/**
			 * The type of physics status event.
			 */
			enum EventType 
			{
				/**
				 * Event fired when there were no active physics objects and at least one is now active.
				 */
				kActivated,

				/**
				 * Event fired when there are no more active physics objects in the world.
				 */
				kDeactivated
			};

			/**
			 * Handles when a physics world status event occurs.
			 */
			virtual void StatusEvent(EventType type) = 0;

		protected:

			/**
			 * Destructor.
			 */
			virtual ~Listener();
		};

		/**
		 * Structure that stores hit test results for ray and sweep tests.
		 */
		struct HitResult
		{
			/**
			 * The collision object that was hit.
			 */
			PhysicsCollisionObject * object;

			/**
			 * The point where the collision occurred, in world space.
			 */
			Vector3 point;

			/**
			 * The fraction (0-1) of the test distance to the collision point.
			 */
			float fraction;

			/**
			 * The normal vector of the collision surface, in world space.
			 */
			Vector3 normal;
		};

		/**
		 * Class that can be overridden to provide custom hit test filters for ray
		 * and sweep tests.
		 *
		 * The default implementation of this class returns only the closest object
		 * that intersects a ray of volume.
		 */
		class HitFilter
		{
		public:

			/**
			 * Constructor.
			 */
			HitFilter();

			/**
			 * Virtual destructor.
			 */
			virtual ~HitFilter();

			/**
			 * Called before performing a hit test with an object to determine
			 * whether or not the object should be tested.
			 *
			 * @param object Object to be queried.
			 * 
			 * @return True if the object should be filtered out, or false to include the object in the test (default).
			 */
			virtual bool Filter(PhysicsCollisionObject * object);

			/**
			 * Called when a ray or sweep test collides with a collision object.
			 *
			 * Each collision object that is hit during the ray or sweep test is passed
			 * to this method, along with details of the hit result. Returning true to
			 * this method will continue with normal hit test processing, where only
			 * closer objects are returned. Returning false results in this method being
			 * called for all objects that intersect the ray or volume.
			 *
			 * @param result HitResult object containing information about the hit.
			 * 
			 * @return True (default) to continue with default behavior where closer
			 *      objects are processed, false to process all intersecting objects.
			 */
			virtual bool Hit(const HitResult& result);
		};

		PhysicsController();
		~PhysicsController();

		bool Initialize();
		void Deinitialize();
		void Update(float elapsed_time);

		/**
		 * Adds a listener to the physics controller.
		 * 
		 * @param listener The listener to add.
		 */
		void AddStatusListener(Listener * listener);

		/**
		 * Removes a listener from the physics controller.
		 * 
		 * @param listener The listener to remove.
		 */
		void RemoveStatusListener(Listener * listener);

		/**
		 * Creates a fixed constraint.
		 * 
		 * @param a The first (possibly only) rigid body to constrain. If this is the only rigid
		 *      body specified the constraint applies between it and the global physics world object.
		 * @param b The second rigid body to constrain (optional).
		 * @return Pointer to the created PhysicsFixedConstraint object.
		 */
		PhysicsFixedConstraint* CreateFixedConstraint(PhysicsRigidBody* a, PhysicsRigidBody* b = nullptr);

		/**
		 * Creates a generic constraint so that the rigid body (or bodies) is
		 * (are) constrained to its (their) current world position(s).
		 * 
		 * @param a The first (possibly only) rigid body to constrain. If this is the only rigid
		 *      body specified the constraint applies between it and the global physics world object.
		 * @param b The second rigid body to constrain (optional).
		 * @return Pointer to the created PhysicsGenericConstraint object.
		 */
		PhysicsGenericConstraint* CreateGenericConstraint(PhysicsRigidBody* a, PhysicsRigidBody* b = nullptr);

		/**
		 * Creates a generic constraint.
		 * 
		 * @param a The first (possibly only) rigid body to constrain. If this is the only rigid
		 *      body specified the constraint applies between it and the global physics world object.
		 * @param rotationOffsetA The rotation offset for the first rigid body 
		 *      (in its local space) with respect to the constraint joint.
		 * @param translationOffsetA The translation offset for the first rigid body
		 *      (in its local space) with respect to the constraint joint.
		 * @param b The second rigid body to constrain (optional).
		 * @param rotationOffsetB The rotation offset for the second rigid body
		 *      (in its local space) with respect to the constraint joint (optional).
		 * @param translationOffsetB The translation offset for the second rigid body
		 *      (in its local space) with respect to the constraint joint (optional).
		 * @return Pointer to the created PhysicsGenericConstraint object.
		 */
		PhysicsGenericConstraint* CreateGenericConstraint(PhysicsRigidBody* a, const Quaternion& rotationOffsetA, const Vector3& translationOffsetA, 
														  PhysicsRigidBody* b = nullptr, const Quaternion& rotationOffsetB = Quaternion(), const Vector3& translationOffsetB = Vector3());

		/**
		 * Creates a hinge constraint.
		 * 
		 * @param a The first (possibly only) rigid body to constrain. If this is the only rigid
		 *      body specified the constraint applies between it and the global physics world object.
		 * @param rotationOffsetA The rotation offset for the first rigid body 
		 *      (in its local space) with respect to the constraint joint.
		 * @param translationOffsetA The translation offset for the first rigid body
		 *      (in its local space) with respect to the constraint joint.
		 * @param b The second rigid body to constrain (optional).
		 * @param rotationOffsetB The rotation offset for the second rigid body
		 *      (in its local space) with respect to the constraint joint (optional).
		 * @param translationOffsetB The translation offset for the second rigid body
		 *      (in its local space) with respect to the constraint joint (optional).
		 * @return Pointer to the created PhysicsHingeConstraint object.
		 */
		PhysicsHingeConstraint* CreateHingeConstraint(PhysicsRigidBody* a, const Quaternion& rotationOffsetA, const Vector3& translationOffsetA,
													  PhysicsRigidBody* b = nullptr, const Quaternion& rotationOffsetB = Quaternion(), const Vector3& translationOffsetB = Vector3());

		/**
		 * Creates a socket constraint so that the rigid body (or bodies) is
		 * (are) constrained using its (their) current world position(s) for
		 * the translation offset(s) to the constraint.
		 * 
		 * @param a The first (possibly only) rigid body to constrain. If this is the only rigid
		 *      body specified the constraint applies between it and the global physics world object.
		 * @param b The second rigid body to constrain (optional).
		 * @return Pointer to the created PhysicsSocketConstraint object.
		 */
		PhysicsSocketConstraint* CreateSocketConstraint(PhysicsRigidBody* a, PhysicsRigidBody* b = nullptr);

		/**
		 * Creates a socket constraint.
		 * 
		 * @param a The first (possibly only) rigid body to constrain. If this is the only rigid
		 *      body specified the constraint applies between it and the global physics world object.
		 * @param translationOffsetA The translation offset for the first rigid body
		 *      (in its local space) with respect to the constraint joint.
		 * @param b The second rigid body to constrain (optional).
		 * @param translationOffsetB The translation offset for the second rigid body
		 *      (in its local space) with respect to the constraint joint (optional).
		 * @return Pointer to the created PhysicsSocketConstraint object.
		 */
		PhysicsSocketConstraint* CreateSocketConstraint(PhysicsRigidBody* a, const Vector3& translationOffsetA,
														PhysicsRigidBody* b = nullptr, const Vector3& translationOffsetB = Vector3());

		/**
		 * Creates a spring constraint so that the rigid body (or bodies) is
		 * (are) constrained using its (their) current world position(s) for
		 * the translation offset(s) to the constraint.
		 * 
		 * @param a The first (possibly only) rigid body to constrain. If this is the only rigid
		 *      body specified the constraint applies between it and the global physics world object.
		 * @param b The second rigid body to constrain (optional).
		 * @return Pointer to the created PhysicsSpringConstraint object.
		 */
		PhysicsSpringConstraint* CreateSpringConstraint(PhysicsRigidBody* a, PhysicsRigidBody* b);

		/**
		 * Creates a spring constraint.
		 * 
		 * @param a The first (possibly only) rigid body to constrain. If this is the only rigid
		 *      body specified the constraint applies between it and the global physics world object.
		 * @param rotationOffsetA The rotation offset for the first rigid body 
		 *      (in its local space) with respect to the constraint joint.
		 * @param translationOffsetA The translation offset for the first rigid body
		 *      (in its local space) with respect to the constraint joint.
		 * @param b The second rigid body to constrain (optional).
		 * @param rotationOffsetB The rotation offset for the second rigid body
		 *      (in its local space) with respect to the constraint joint (optional).
		 * @param translationOffsetB The translation offset for the second rigid body
		 *      (in its local space) with respect to the constraint joint (optional).
		 * @return Pointer to the created PhysicsSpringConstraint object.
		 */
		PhysicsSpringConstraint* CreateSpringConstraint(PhysicsRigidBody* a, const Quaternion& rotationOffsetA, const Vector3& translationOffsetA,          
														PhysicsRigidBody* b, const Quaternion& rotationOffsetB, const Vector3& translationOffsetB);

		/**
		 * Sets the gravity vector for the simulated physics world.
		 * 
		 * @param gravity The gravity vector.
		 */
		void SetGravity(const Vector3& gravity);

		/**
		 * Gets the gravity vector for the simulated physics world.
		 * 
		 * @return The gravity vector.
		 */
		const Vector3& GetGravity() const;

		/**
		 * Performs a ray test on the physics world.
		 * 
		 * @param ray The ray to test intersection with.
		 * @param distance How far along the given ray to test for intersections.
		 * @param result Optional pointer to a HitTest structure to store the hit test result information in.
		 *      When using a default (or no) filter, this will always be the closest object hit. Otherwise, if 
		 *      using a custom filter, it will be the last object passed to the HitFilter::hit method (which
		 *      is not necessarily the closest or furthest).
		 * @param filter Optional filter pointer used to control which objects are tested.
		 *
		 * @return True if the ray test collided with a physics object, false otherwise.
		 */
		bool RayTest(const Ray& ray, float distance, HitResult* result = nullptr, HitFilter* filter = nullptr);

		/**
		 * Performs a sweep test of the given collision object on the physics world.
		 *
		 * The start position of the sweep test is defined by the current world position
		 * of the specified collision object.
		 *
		 * @param object The collision object to test.
		 * @param endPosition The end position of the sweep test, in world space.
		 * @param result Optional pointer to a HitTest structure to store the hit test result information in.
		 *      When using a default (or no) filter, this will always be the closest object hit. Otherwise, if 
		 *      using a custom filter, it will be the last object passed to the HitFilter::hit method (which
		 *      is not necessarily the closest or furthest).
		 * @param filter Optional filter pointer used to control which objects are tested.
		 * 
		 * @return True if the object intersects any other physics objects, false otherwise.
		 */
		bool SweepTest(PhysicsCollisionObject* object, const Vector3& endPosition, HitResult* result = nullptr, HitFilter* filter = nullptr);

	protected:

		/**
		 * Internal class used to integrate with Bullet collision callbacks.
		 */
		class CollisionCallback : public btCollisionWorld::ContactResultCallback {
		public:
			/**
			 * Constructor.
			 * 
			 * @param pc The physics controller that owns the callback.
			 */
			CollisionCallback(PhysicsController * controller) : controller_(controller) {}

		protected:
			/**
			 * Internal function used for Bullet integration (do not use or override).
			 */
			btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* a, int partIdA, int indexA, const btCollisionObjectWrapper* b, int partIdB, int indexB);    

		private:
			PhysicsController * controller_;
		};

		//! Struct that holds enum representing collision status
		struct CollisionStatus
		{
			enum {
				kDirty      = 0x01,
				kCollision  = 0x02,
				kRegistered = 0x04,
				kRemove     = 0x08
			};
		};

		// Represents the collision listeners and status for a given collision pair (used by the collision status cache).
		struct CollisionInfo
		{
			CollisionInfo() : status(0) { }

			std::vector<PhysicsCollisionObject::CollisionListener*> listeners;
			int status;
		};

		// Adds the given collision listener for the two given collision objects.
		void AddCollisionListener(PhysicsCollisionObject::CollisionListener * listener, PhysicsCollisionObject * objectA, PhysicsCollisionObject * objectB);

		// Removes the given collision listener.
		void RemoveCollisionListener(PhysicsCollisionObject::CollisionListener * listener, PhysicsCollisionObject * objectA, PhysicsCollisionObject * objectB);

		// Adds the given collision object to the world.
		void AddCollisionObject(PhysicsCollisionObject * object);
		
		// Removes the given collision object from the simulated physics world.
		void RemoveCollisionObject(PhysicsCollisionObject * object, bool removeListeners);

		// Gets the corresponding Scythe object for the given Bullet object.
		PhysicsCollisionObject * GetCollisionObject(const btCollisionObject* collisionObject) const;

		// Creates a collision shape for the given node and gameplay shape definition.
		// Populates 'centerOfMassOffset' with the correct calculated center of mass offset.
		PhysicsCollisionShape* CreateShape(Node* node, const PhysicsCollisionShape::Definition& shape, Vector3* centerOfMassOffset, bool dynamic);

		// Creates a box collision shape.
		PhysicsCollisionShape* CreateBox(const Vector3& extents, const Vector3& scale);

		// Creates a sphere collision shape.
		PhysicsCollisionShape* CreateSphere(float radius, const Vector3& scale);

		// Creates a capsule collision shape.
		PhysicsCollisionShape* CreateCapsule(float radius, float height, const Vector3& scale);

		// Creates a triangle mesh collision shape.
		PhysicsCollisionShape* CreateMesh(Mesh* mesh, const Vector3& scale, bool dynamic);

		// Destroys a collision shape created through PhysicsController
		void DestroyShape(PhysicsCollisionShape* shape);

		//! Sets up the given constraint for the given two rigid bodies.
		void AddConstraint(PhysicsRigidBody * a, PhysicsRigidBody * b, PhysicsConstraint * constraint);

		//! Checks whether constraints are supported for the given rigid bodies and emits a warning if they are not.
		bool CheckConstraintRigidBodies(PhysicsRigidBody * a, PhysicsRigidBody * b);

		//! Removes the given constraint from the simulated physics world.
		void RemoveConstraint(PhysicsConstraint * constraint);

	private:
		btDefaultCollisionConfiguration * collision_configuration_;
		btCollisionDispatcher * dispatcher_;
		btBroadphaseInterface * broadphase_;
		btSequentialImpulseConstraintSolver * solver_;
		btDynamicsWorld * world_;
		btGhostPairCallback * ghost_pair_callback_;

		std::vector<PhysicsCollisionShape*> shapes_;
		Listener::EventType status_;
		std::vector<Listener*>* listeners_;

		Vector3 gravity_;

		std::map<PhysicsCollisionObject::CollisionPair, CollisionInfo> collision_status_;
		CollisionCallback * collision_callback_;

		bool is_updating_;
	};

} // namespace scythe

#endif