#include "physics_controller.h"

#include "physics_fixed_constraint.h"
#include "physics_generic_constraint.h"
#include "physics_hinge_constraint.h"
#include "physics_socket_constraint.h"
#include "physics_spring_constraint.h"

#include "math/bounding_box.h"
#include "math/bounding_sphere.h"
#include "math/common_math.h"

#include "common/sc_assert.h"
#include "common/sc_delete.h"
#include "common/log.h"

#include "model/model.h"
#include "model/mesh.h"
#include "model/mesh_parts_enumerator.h"

#ifdef SC_USE_MEM_LEAK_DETECTION
# undef new
#endif
#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"
#ifdef SC_USE_MEM_LEAK_DETECTION
# define new DEBUG_NEW
#endif

#include <cmath>

namespace scythe {

	PhysicsController::PhysicsController()
	: collision_configuration_(nullptr)
	, dispatcher_(nullptr)
	, broadphase_(nullptr)
	, solver_(nullptr)
	, world_(nullptr)
	, ghost_pair_callback_(nullptr)
	, shapes_()
	, status_(PhysicsController::Listener::kDeactivated)
	, listeners_(nullptr)
	, gravity_(0.0f, -9.81f, 0.0f)
	, collision_callback_(nullptr)
	, is_updating_(false)
	{
		collision_callback_ = new CollisionCallback(this);
	}
	PhysicsController::~PhysicsController()
	{
		SC_SAFE_DELETE(collision_callback_);
		SC_SAFE_DELETE(ghost_pair_callback_);
		SC_SAFE_DELETE(listeners_);
	}
	bool PhysicsController::Initialize()
	{
		collision_configuration_ = new btDefaultCollisionConfiguration();
		dispatcher_ = new btCollisionDispatcher(collision_configuration_);
		broadphase_ = new btDbvtBroadphase();
		solver_ = new btSequentialImpulseConstraintSolver();

		// Create the world.
		world_ = new btDiscreteDynamicsWorld(dispatcher_, broadphase_, solver_, collision_configuration_);
		world_->setGravity(BV(gravity_));

		// Register ghost pair callback so bullet detects collisions with ghost objects (used for character collisions).
		SC_ASSERT(world_->getPairCache());
		ghost_pair_callback_ = new btGhostPairCallback();
		world_->getPairCache()->setInternalGhostPairCallback(ghost_pair_callback_);
		world_->getDispatchInfo().m_allowedCcdPenetration = 0.0001f;

		return true;
	}
	void PhysicsController::Deinitialize()
	{
		// Clean up the world and its various components.
		SC_SAFE_DELETE(world_);
		SC_SAFE_DELETE(ghost_pair_callback_);
		SC_SAFE_DELETE(solver_);
		SC_SAFE_DELETE(broadphase_);
		SC_SAFE_DELETE(dispatcher_);
		SC_SAFE_DELETE(collision_configuration_);
	}
	void PhysicsController::Update(float elapsed_time)
	{
		SC_ASSERT(world_);
		is_updating_ = true;

		// Update the physics simulation, with a maximum
		// of 10 simulation steps being performed in a given frame.
		world_->stepSimulation(elapsed_time, 10);

		// If we have status listeners, then check if our status has changed.
		if (listeners_ /*|| hasScriptListener(GP_GET_SCRIPT_EVENT(PhysicsController, statusEvent))*/)
		{
			Listener::EventType old_status = status_;

			if (status_ == Listener::kDeactivated)
			{
				for (int i = 0; i < world_->getNumCollisionObjects(); ++i)
				{
					SC_ASSERT(world_->getCollisionObjectArray()[i]);
					if (world_->getCollisionObjectArray()[i]->isActive())
					{
						status_ = Listener::kActivated;
						break;
					}
				}
			}
			else
			{
				bool all_inactive = true;
				for (int i = 0; i < world_->getNumCollisionObjects(); ++i)
				{
					SC_ASSERT(world_->getCollisionObjectArray()[i]);
					if (world_->getCollisionObjectArray()[i]->isActive())
					{
						all_inactive = false;
						break;
					}
				}

				if (all_inactive)
					status_ = Listener::kDeactivated;
			}

			// If the status has changed, notify our listeners.
			if (old_status != status_)
			{
				if (listeners_)
				{
					for (unsigned int k = 0; k < listeners_->size(); ++k)
					{
						SC_ASSERT((*listeners_)[k]);
						(*listeners_)[k]->StatusEvent(status_);
					}
				}

				//fireScriptEvent<void>(GP_GET_SCRIPT_EVENT(PhysicsController, statusEvent), status_);
			}
		}

		// All statuses are set with the DIRTY bit before collision processing occurs.
		// During collision processing, if a collision occurs, the status is 
		// set to COLLISION and the DIRTY bit is cleared. Then, after collision processing 
		// is finished, if a given status is still dirty, the COLLISION bit is cleared.
		//
		// If an entry was marked for removal in the last frame, fire NOT_COLLIDING if appropriate and remove it now.

		// Dirty the collision status cache entries.
		std::map<PhysicsCollisionObject::CollisionPair, CollisionInfo>::iterator iter = collision_status_.begin();
		for (; iter != collision_status_.end();)
		{
			if ((iter->second.status & CollisionStatus::kRemove) != 0)
			{
				if ((iter->second.status & CollisionStatus::kCollision) != 0 && iter->first.objectB)
				{
					size_t size = iter->second.listeners.size();
					for (size_t i = 0; i < size; i++)
					{
						PhysicsCollisionObject::CollisionPair cp(iter->first.objectA, NULL);
						iter->second.listeners[i]->CollisionEvent(PhysicsCollisionObject::CollisionListener::kNotColliding, cp);
					}
				}

				std::map<PhysicsCollisionObject::CollisionPair, CollisionInfo>::iterator eraseIter = iter;
				iter++;
				collision_status_.erase(eraseIter);
			}
			else
			{
				iter->second.status |= CollisionStatus::kDirty;
				iter++;
			}
		}

		// Go through the collision status cache and perform all registered collision tests.
		iter = collision_status_.begin();
		for (; iter != collision_status_.end(); iter++)
		{
			// If this collision pair was one that was registered for listening, then perform the collision test.
			// (In the case where we register for all collisions with a rigid body, there will be a lot
			// of collision pairs in the status cache that we did not explicitly register for.)
			if ((iter->second.status & CollisionStatus::kRegistered) != 0 &&
				(iter->second.status & CollisionStatus::kRemove) == 0)
			{
				if (iter->first.objectB)
					world_->contactPairTest(iter->first.objectA->GetCollisionObject(), iter->first.objectB->GetCollisionObject(), *collision_callback_);
				else
					world_->contactTest(iter->first.objectA->GetCollisionObject(), *collision_callback_);
			}
		}

		// Update all the collision status cache entries.
		iter = collision_status_.begin();
		for ( ; iter != collision_status_.end(); iter++)
		{
			if ((iter->second.status & CollisionStatus::kDirty) != 0)
			{
				if ((iter->second.status & CollisionStatus::kCollision) != 0 && iter->first.objectB)
				{
					size_t size = iter->second.listeners.size();
					for (size_t i = 0; i < size; i++)
					{
						iter->second.listeners[i]->CollisionEvent(PhysicsCollisionObject::CollisionListener::kNotColliding, iter->first);
					}
				}

				iter->second.status &= ~CollisionStatus::kCollision;
			}
		}

		is_updating_ = false;
	}
	void PhysicsController::AddStatusListener(Listener * listener)
	{
		SC_ASSERT(listener);
		if (!listeners_)
			listeners_ = new std::vector<Listener*>();

		listeners_->push_back(listener);
	}

	void PhysicsController::RemoveStatusListener(Listener * listener)
	{
		SC_ASSERT(listener);
		if (!listeners_)
			return;

		for (std::vector<Listener*>::iterator iter = listeners_->begin(); iter != listeners_->end(); ++iter)
		{
			if (*iter == listener)
			{
				listeners_->erase(iter);
				return;
			}
		}
	}
	PhysicsFixedConstraint* PhysicsController::CreateFixedConstraint(PhysicsRigidBody* a, PhysicsRigidBody* b)
	{
		CheckConstraintRigidBodies(a, b);
		PhysicsFixedConstraint* constraint = new PhysicsFixedConstraint(a, b);
		AddConstraint(a, b, constraint);
		return constraint;
	}
	PhysicsGenericConstraint* PhysicsController::CreateGenericConstraint(PhysicsRigidBody* a, PhysicsRigidBody* b)
	{
		CheckConstraintRigidBodies(a, b);
		PhysicsGenericConstraint* constraint = new PhysicsGenericConstraint(a, b);
		AddConstraint(a, b, constraint);
		return constraint;
	}
	PhysicsGenericConstraint* PhysicsController::CreateGenericConstraint(PhysicsRigidBody* a,
		const Quaternion& rotationOffsetA, const Vector3& translationOffsetA, PhysicsRigidBody* b,
		const Quaternion& rotationOffsetB, const Vector3& translationOffsetB)
	{
		CheckConstraintRigidBodies(a, b);
		PhysicsGenericConstraint* constraint = new PhysicsGenericConstraint(a, rotationOffsetA, translationOffsetA, b, rotationOffsetB, translationOffsetB);
		AddConstraint(a, b, constraint);
		return constraint;
	}
	PhysicsHingeConstraint* PhysicsController::CreateHingeConstraint(PhysicsRigidBody* a,
		const Quaternion& rotationOffsetA, const Vector3& translationOffsetA, PhysicsRigidBody* b, 
		const Quaternion& rotationOffsetB, const Vector3& translationOffsetB)
	{
		CheckConstraintRigidBodies(a, b);
		PhysicsHingeConstraint* constraint = new PhysicsHingeConstraint(a, rotationOffsetA, translationOffsetA, b, rotationOffsetB, translationOffsetB);
		AddConstraint(a, b, constraint);
		return constraint;
	}
	PhysicsSocketConstraint* PhysicsController::CreateSocketConstraint(PhysicsRigidBody* a, PhysicsRigidBody* b)
	{
		CheckConstraintRigidBodies(a, b);
		PhysicsSocketConstraint* constraint = new PhysicsSocketConstraint(a, b);
		AddConstraint(a, b, constraint);
		return constraint;
	}
	PhysicsSocketConstraint* PhysicsController::CreateSocketConstraint(PhysicsRigidBody* a,
		const Vector3& translationOffsetA, PhysicsRigidBody* b, const Vector3& translationOffsetB)
	{
		CheckConstraintRigidBodies(a, b);
		PhysicsSocketConstraint* constraint = new PhysicsSocketConstraint(a,translationOffsetA, b, translationOffsetB);
		AddConstraint(a, b, constraint);
		return constraint;
	}
	PhysicsSpringConstraint* PhysicsController::CreateSpringConstraint(PhysicsRigidBody* a, PhysicsRigidBody* b)
	{
		CheckConstraintRigidBodies(a, b);
		PhysicsSpringConstraint* constraint = new PhysicsSpringConstraint(a, b);
		AddConstraint(a, b, constraint);
		return constraint;
	}
	PhysicsSpringConstraint* PhysicsController::CreateSpringConstraint(PhysicsRigidBody* a, const Quaternion& rotationOffsetA, const Vector3& translationOffsetA,           
																	   PhysicsRigidBody* b, const Quaternion& rotationOffsetB, const Vector3& translationOffsetB)
	{
		CheckConstraintRigidBodies(a, b);
		PhysicsSpringConstraint* constraint = new PhysicsSpringConstraint(a, rotationOffsetA, translationOffsetA, b, rotationOffsetB, translationOffsetB);
		AddConstraint(a, b, constraint);
		return constraint;
	}
	bool PhysicsController::RayTest(const Ray& ray, float distance, HitResult * result, HitFilter * filter)
	{
		class RayTestCallback : public btCollisionWorld::ClosestRayResultCallback {
		public:

			RayTestCallback(const btVector3& rayFromWorld, const btVector3& rayToWorld, HitFilter* filter)
			: btCollisionWorld::ClosestRayResultCallback(rayFromWorld, rayToWorld)
			, filter(filter)
			{
			}

			virtual bool needsCollision(btBroadphaseProxy* proxy0) const
			{
				if (!btCollisionWorld::ClosestRayResultCallback::needsCollision(proxy0))
					return false;

				btCollisionObject* co = reinterpret_cast<btCollisionObject*>(proxy0->m_clientObject);
				PhysicsCollisionObject* object = reinterpret_cast<PhysicsCollisionObject*>(co->getUserPointer());
				if (object == NULL)
					return false;

				return filter ? !filter->Filter(object) : true;
			}

			btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
			{
				SC_ASSERT(rayResult.m_collisionObject);
				PhysicsCollisionObject* object = reinterpret_cast<PhysicsCollisionObject*>(rayResult.m_collisionObject->getUserPointer());

				if (object == NULL)
					return 1.0f; // ignore

				float result = btCollisionWorld::ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);

				hitResult.object = object;
				hitResult.point.Set(m_hitPointWorld.x(), m_hitPointWorld.y(), m_hitPointWorld.z());
				hitResult.fraction = m_closestHitFraction;
				hitResult.normal.Set(m_hitNormalWorld.x(), m_hitNormalWorld.y(), m_hitNormalWorld.z());

				if (filter && !filter->Hit(hitResult))
					return 1.0f; // process next collision

				return result; // continue normally
			}

		private:

			HitFilter * filter;
			HitResult hitResult;
		};

		SC_ASSERT(world_);

		btVector3 rayFromWorld(BV(ray.GetOrigin()));
		btVector3 rayToWorld(rayFromWorld + BV(ray.GetDirection() * distance));

		RayTestCallback callback(rayFromWorld, rayToWorld, filter);
		world_->rayTest(rayFromWorld, rayToWorld, callback);
		if (callback.hasHit())
		{
			if (result)
			{
				result->object = GetCollisionObject(callback.m_collisionObject);
				result->point.Set(callback.m_hitPointWorld.x(), callback.m_hitPointWorld.y(), callback.m_hitPointWorld.z());
				result->fraction = callback.m_closestHitFraction;
				result->normal.Set(callback.m_hitNormalWorld.x(), callback.m_hitNormalWorld.y(), callback.m_hitNormalWorld.z());
			}

			return true;
		}

		return false;
	}

	bool PhysicsController::SweepTest(PhysicsCollisionObject * object, const Vector3& endPosition, HitResult* result, HitFilter* filter)
	{
		class SweepTestCallback : public btCollisionWorld::ClosestConvexResultCallback {
		public:

			SweepTestCallback(PhysicsCollisionObject* me, PhysicsController::HitFilter* filter)
			: btCollisionWorld::ClosestConvexResultCallback(btVector3(0.0f, 0.0f, 0.0f), btVector3(0.0f, 0.0f, 0.0f))
			, me(me)
			, filter(filter)
			{
			}

			virtual bool needsCollision(btBroadphaseProxy* proxy0) const
			{
				if (!btCollisionWorld::ClosestConvexResultCallback::needsCollision(proxy0))
					return false;

				btCollisionObject* co = reinterpret_cast<btCollisionObject*>(proxy0->m_clientObject);
				PhysicsCollisionObject* object = reinterpret_cast<PhysicsCollisionObject*>(co->getUserPointer());
				if (object == NULL || object == me)
					return false;

				return filter ? !filter->Filter(object) : true;
			}

			btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace)
			{
				SC_ASSERT(convexResult.m_hitCollisionObject);
				PhysicsCollisionObject* object = reinterpret_cast<PhysicsCollisionObject*>(convexResult.m_hitCollisionObject->getUserPointer());

				if (object == NULL)
					return 1.0f;

				btScalar result = ClosestConvexResultCallback::addSingleResult(convexResult, normalInWorldSpace);

				hitResult.object = object;
				hitResult.point.Set(m_hitPointWorld.x(), m_hitPointWorld.y(), m_hitPointWorld.z());
				hitResult.fraction = m_closestHitFraction;
				hitResult.normal.Set(m_hitNormalWorld.x(), m_hitNormalWorld.y(), m_hitNormalWorld.z());

				if (filter && !filter->Hit(hitResult))
					return 1.0f;

				return result;
			}

		private:

			PhysicsCollisionObject * me;
			PhysicsController::HitFilter* filter;
			PhysicsController::HitResult hitResult;
		};

		SC_ASSERT(object && object->GetCollisionShape());
		PhysicsCollisionShape* shape = object->GetCollisionShape();
		PhysicsCollisionShape::Type type = shape->type();
		if (type != PhysicsCollisionShape::kBox &&
			type != PhysicsCollisionShape::kSphere &&
			type != PhysicsCollisionShape::kCapsule)
			return false; // unsupported type

		// Define the start transform.
		btTransform start;
		start.setIdentity();
		if (object->GetNode())
		{
			Vector3 translation;
			Quaternion rotation;
			const Matrix4& m = object->GetNode()->GetWorldMatrix();
			m.GetTranslation(&translation);
			m.GetRotation(&rotation);

			start.setIdentity();
			start.setOrigin(BV(translation));
			start.setRotation(BQ(rotation));
		}

		// Define the end transform.
		btTransform end(start);
		end.setOrigin(BV(endPosition));

		// Perform bullet convex sweep test.
		SweepTestCallback callback(object, filter);

		// If the object is represented by a ghost object, use the ghost object's convex sweep test
		// since it is much faster than the world's version.
		// NOTE: Unfortunately the ghost object sweep test does not seem reliable here currently, so using world's version instead.
		/*switch (object->getType())
		{
		case PhysicsCollisionObject::GHOST_OBJECT:
		case PhysicsCollisionObject::CHARACTER:
			static_cast<PhysicsGhostObject*>(object)->_ghostObject->convexSweepTest(static_cast<btConvexShape*>(shape->getShape()), start, end, callback, world_->getDispatchInfo().m_allowedCcdPenetration);
			break;

		default:
			world_->convexSweepTest(static_cast<btConvexShape*>(shape->getShape()), start, end, callback, world_->getDispatchInfo().m_allowedCcdPenetration);
			break;
		}*/

		SC_ASSERT(world_);
		world_->convexSweepTest(static_cast<btConvexShape*>(shape->shape()), start, end, callback, world_->getDispatchInfo().m_allowedCcdPenetration);

		// Check for hits and store results.
		if (callback.hasHit())
		{
			if (result)
			{
				result->object = GetCollisionObject(callback.m_hitCollisionObject);
				result->point.Set(callback.m_hitPointWorld.x(), callback.m_hitPointWorld.y(), callback.m_hitPointWorld.z());
				result->fraction = callback.m_closestHitFraction;
				result->normal.Set(callback.m_hitNormalWorld.x(), callback.m_hitNormalWorld.y(), callback.m_hitNormalWorld.z());
			}

			return true;
		}

		return false;
	}
	btScalar PhysicsController::CollisionCallback::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* a, int partIdA, int indexA, 
		const btCollisionObjectWrapper* b, int partIdB, int indexB)
	{
		SC_ASSERT(controller_);

		// Get pointers to the PhysicsCollisionObject objects.
		PhysicsCollisionObject* objectA = controller_->GetCollisionObject(a->m_collisionObject);
		PhysicsCollisionObject* objectB = controller_->GetCollisionObject(b->m_collisionObject);

		// If the given collision object pair has collided in the past, then
		// we notify the listeners only if the pair was not colliding
		// during the previous frame. Otherwise, it's a new pair, so add a
		// new entry to the cache with the appropriate listeners and notify them.
		PhysicsCollisionObject::CollisionPair pair(objectA, objectB);

		CollisionInfo * collisionInfo;
		if (controller_->collision_status_.count(pair) > 0)
		{
			collisionInfo = &controller_->collision_status_[pair];
		}
		else
		{
			// Add a new collision pair for these objects.
			collisionInfo = &controller_->collision_status_[pair];

			// Add the appropriate listeners.
			PhysicsCollisionObject::CollisionPair p1(pair.objectA, NULL);
			if (controller_->collision_status_.count(p1) > 0)
			{
				const CollisionInfo& ci = controller_->collision_status_[p1];
				std::vector<PhysicsCollisionObject::CollisionListener*>::const_iterator iter = ci.listeners.begin();
				for (; iter != ci.listeners.end(); iter++)
				{
					SC_ASSERT(*iter);
					collisionInfo->listeners.push_back(*iter);
				}
			}
			PhysicsCollisionObject::CollisionPair p2(pair.objectB, NULL);
			if (controller_->collision_status_.count(p2) > 0)
			{
				const CollisionInfo& ci = controller_->collision_status_[p2];
				std::vector<PhysicsCollisionObject::CollisionListener*>::const_iterator iter = ci.listeners.begin();
				for (; iter != ci.listeners.end(); iter++)
				{
					SC_ASSERT(*iter);
					collisionInfo->listeners.push_back(*iter);
				}
			}
		}

		// Fire collision event.
		if ((collisionInfo->status & CollisionStatus::kCollision) == 0)
		{
			std::vector<PhysicsCollisionObject::CollisionListener*>::const_iterator iter = collisionInfo->listeners.begin();
			for (; iter != collisionInfo->listeners.end(); iter++)
			{
				SC_ASSERT(*iter);
				if ((collisionInfo->status & CollisionStatus::kRemove) == 0)
				{
					(*iter)->CollisionEvent(PhysicsCollisionObject::CollisionListener::kColliding, pair, Vector3(cp.getPositionWorldOnA().x(), cp.getPositionWorldOnA().y(), cp.getPositionWorldOnA().z()),
						Vector3(cp.getPositionWorldOnB().x(), cp.getPositionWorldOnB().y(), cp.getPositionWorldOnB().z()));
				}
			}
		}

		// Update the collision status cache (we remove the dirty bit
		// set in the controller's update so that this particular collision pair's
		// status is not reset to 'no collision' when the controller's update completes).
		collisionInfo->status &= ~CollisionStatus::kDirty;
		collisionInfo->status |= CollisionStatus::kCollision;
		return 0.0f;
	}
	void PhysicsController::AddCollisionListener(PhysicsCollisionObject::CollisionListener* listener, PhysicsCollisionObject* objectA, PhysicsCollisionObject* objectB)
	{
		SC_ASSERT(listener);
		
		// One of the collision objects in the pair must be non-null.
		SC_ASSERT(objectA || objectB);
		PhysicsCollisionObject::CollisionPair pair(objectA, objectB);

		// Add the listener and ensure the status includes that this collision pair is registered.
		CollisionInfo& info = collision_status_[pair];
		info.listeners.push_back(listener);
		info.status |= CollisionStatus::kRegistered;
	}
	void PhysicsController::RemoveCollisionListener(PhysicsCollisionObject::CollisionListener* listener, PhysicsCollisionObject* objectA, PhysicsCollisionObject* objectB)
	{
		// One of the collision objects in the pair must be non-null.
		SC_ASSERT(objectA || objectB);
		PhysicsCollisionObject::CollisionPair pair(objectA, objectB);

		// Mark the collision pair for these objects for removal.
		if (collision_status_.count(pair) > 0)
		{
			collision_status_[pair].status |= CollisionStatus::kRemove;
		}
	}
	void PhysicsController::AddCollisionObject(PhysicsCollisionObject* object)
	{
		SC_ASSERT(object && object->GetCollisionObject());
		SC_ASSERT(world_);

		// Assign user pointer for the bullet collision object to allow efficient
		// lookups of bullet objects -> gameplay objects.
		object->GetCollisionObject()->setUserPointer(object);
		short group = (short)object->group_;
		short mask = (short)object->mask_;

		// Add the object to the physics world.
		switch (object->type())
		{
		case PhysicsCollisionObject::kRigidBody:
			world_->addRigidBody(static_cast<btRigidBody*>(object->GetCollisionObject()), group, mask);
			break;

		case PhysicsCollisionObject::kCharacter:
		case PhysicsCollisionObject::kGhostObject:
			world_->addCollisionObject(object->GetCollisionObject(), group, mask);
			break;

		default:
			SC_ERROR("Unsupported collision object type (%d).", object->type());
			break;
		}
	}
	void PhysicsController::RemoveCollisionObject(PhysicsCollisionObject * object, bool removeListeners)
	{
		SC_ASSERT(object);
		SC_ASSERT(world_);
		SC_ASSERT(!is_updating_);

		// Remove the collision object from the world.
		if (object->GetCollisionObject())
		{
			switch (object->type())
			{
			case PhysicsCollisionObject::kRigidBody:
				world_->removeRigidBody(static_cast<btRigidBody*>(object->GetCollisionObject()));
				break;

			case PhysicsCollisionObject::kCharacter:
			case PhysicsCollisionObject::kGhostObject:
				world_->removeCollisionObject(object->GetCollisionObject());
				break;

			default:
				SC_ERROR("Unsupported collision object type (%d).", object->type());
				break;
			}
		}

		// Find all references to the object in the collision status cache and mark them for removal.
		if (removeListeners)
		{
			std::map<PhysicsCollisionObject::CollisionPair, CollisionInfo>::iterator iter = collision_status_.begin();
			for (; iter != collision_status_.end(); iter++)
			{
				if (iter->first.objectA == object || iter->first.objectB == object)
					iter->second.status |= CollisionStatus::kRemove;
			}
		}
	}
	PhysicsCollisionObject * PhysicsController::GetCollisionObject(const btCollisionObject* collisionObject) const
	{
		// Scythe collision objects are stored in the userPointer data of Bullet collision objects.
		SC_ASSERT(collisionObject);
		return reinterpret_cast<PhysicsCollisionObject*>(collisionObject->getUserPointer());
	}
	static void GetBoundingBox(Node* node, BoundingBox* out, bool merge = false)
	{
		SC_ASSERT(node);
		SC_ASSERT(out);

		Model* model = dynamic_cast<Model*>(node->GetDrawable());
		if (model != NULL)
		{
			SC_ASSERT(model->GetMesh());

			if (merge)
				out->Merge(model->GetMesh()->GetBoundingBox());
			else
			{
				out->Set(model->GetMesh()->GetBoundingBox());
				merge = true;
			}
		}

		Node * child = node->GetFirstChild();
		while (child)
		{
			GetBoundingBox(child, out, merge);
			child = child->GetNextSibling();
		}
	}
	static void GetBoundingSphere(Node* node, BoundingSphere* out, bool merge = false)
	{
		SC_ASSERT(node);
		SC_ASSERT(out);

		Model* model = dynamic_cast<Model*>(node->GetDrawable());
		if (model != NULL)
		{
			SC_ASSERT(model->GetMesh());

			if (merge)
				out->Merge(model->GetMesh()->GetBoundingSphere());
			else
			{
				out->Set(model->GetMesh()->GetBoundingSphere());
				merge = true;
			}
		}

		Node * child = node->GetFirstChild();
		while (child)
		{
			GetBoundingSphere(child, out, merge);
			child = child->GetNextSibling();
		}
	}
	static void ComputeCenterOfMass(const Vector3& center, const Vector3& scale, Vector3* center_of_mass_offset)
	{
		SC_ASSERT(center_of_mass_offset);

		// Update center of mass offset.
		*center_of_mass_offset = center;
		center_of_mass_offset->x *= scale.x;
		center_of_mass_offset->y *= scale.y;
		center_of_mass_offset->z *= scale.z;
		center_of_mass_offset->Negate();
	}
	PhysicsCollisionShape* PhysicsController::CreateShape(Node* node, const PhysicsCollisionShape::Definition& shape, Vector3* center_of_mass_offset, bool dynamic)
	{
		SC_ASSERT(node);

		PhysicsCollisionShape * collision_shape = nullptr;

		// Get the node's world scale (we need to apply this during creation since rigid bodies don't scale dynamically).
		Vector3 scale;
		node->GetWorldMatrix().GetScale(&scale);

		switch (shape.type)
		{
		case PhysicsCollisionShape::kBox:
			if (shape.is_explicit)
			{
				// Use the passed in box information.
				collision_shape = CreateBox(Vector3(shape.data.box.extents), Vector3::One());

				if (shape.center_absolute)
				{
					ComputeCenterOfMass(Vector3(shape.data.box.center), Vector3::One(), center_of_mass_offset);
				}
				else
				{
					BoundingBox box;
					GetBoundingBox(node, &box);
					ComputeCenterOfMass(box.GetCenter() + Vector3(shape.data.box.center), scale, center_of_mass_offset);
				}
			}
			else
			{
				// Automatically compute bounding box from mesh's bounding box.
				BoundingBox box;
				GetBoundingBox(node, &box);
				Vector3 extents(
					0.5f * std::fabs(box.max.x - box.min.x),
					0.5f * std::fabs(box.max.y - box.min.y),
					0.5f * std::fabs(box.max.z - box.min.z));
				collision_shape = CreateBox(extents, scale);

				ComputeCenterOfMass(box.GetCenter(), scale, center_of_mass_offset);
			}
			break;

		case PhysicsCollisionShape::kSphere:
			if (shape.is_explicit)
			{
				// Use the passed in sphere information.
				collision_shape = CreateSphere(shape.data.sphere.radius, Vector3::One());

				if (shape.center_absolute)
				{
					ComputeCenterOfMass(Vector3(shape.data.sphere.center), Vector3::One(), center_of_mass_offset);
				}
				else
				{
					BoundingSphere sphere;
					GetBoundingSphere(node, &sphere);
					ComputeCenterOfMass(sphere.center + Vector3(shape.data.sphere.center), scale, center_of_mass_offset);
				}
			}
			else
			{
				// Automatically compute bounding sphere from mesh's bounding sphere.
				BoundingSphere sphere;
				GetBoundingSphere(node, &sphere);
				collision_shape = CreateSphere(sphere.radius, scale);

				ComputeCenterOfMass(sphere.center, scale, center_of_mass_offset);
			}
			break;

		case PhysicsCollisionShape::kCapsule:
			if (shape.is_explicit)
			{
				// Use the passed in capsule information.
				collision_shape = CreateCapsule(shape.data.capsule.radius, shape.data.capsule.height, Vector3::One());

				if (shape.center_absolute)
				{
					ComputeCenterOfMass(Vector3(shape.data.capsule.center), Vector3::One(), center_of_mass_offset);
				}
				else
				{
					BoundingBox box;
					GetBoundingBox(node, &box);
					ComputeCenterOfMass(box.GetCenter() + Vector3(shape.data.capsule.center), scale, center_of_mass_offset);
				}
			}
			else
			{
				// Compute a capsule shape that roughly matches the bounding box of the mesh.
				BoundingBox box;
				GetBoundingBox(node, &box);
				float radius = Max<float>((box.max.x - box.min.x) * 0.5f, (box.max.z - box.min.z) * 0.5f);
				float height = box.max.y - box.min.y;
				collision_shape = CreateCapsule(radius, height, scale);

				ComputeCenterOfMass(box.GetCenter(), scale, center_of_mass_offset);
			}
			break;

		// case PhysicsCollisionShape::kHeightfield:
		// 	{
		// 		if (shape.is_explicit)
		// 		{
		// 			// Build heightfield rigid body from the passed in shape.
		// 			collision_shape = CreateHeightfield(node, shape.data.heightfield, center_of_mass_offset);
		// 		}
		// 		else
		// 		{
		// 			// Build the heightfield from an attached terrain's height array
		// 			if (dynamic_cast<Terrain*>(node->getDrawable()) == NULL)
		// 				SC_ERROR("Empty heightfield collision shapes can only be used on nodes that have an attached Terrain.");
		// 			else
		// 				collision_shape = CreateHeightfield(node, dynamic_cast<Terrain*>(node->getDrawable())->_heightfield, center_of_mass_offset);
		// 		}
		// 	}
		// 	break;

		case PhysicsCollisionShape::kMesh:
			// Build mesh from passed in shape.
			collision_shape = CreateMesh(shape.data.mesh, scale, dynamic);
			break;

		default:
			SC_ERROR("Unsupported collision shape type (%d).", shape.type);
			break;
		}

		return collision_shape;
	}
	PhysicsCollisionShape* PhysicsController::CreateBox(const Vector3& extents, const Vector3& scale)
	{
		// Extents originally is always half of bounding box size
		// But Bullet uses half extents as regular extents.
		btVector3 halfExtents(scale.x * extents.x, scale.y * extents.y, scale.z * extents.z);

		PhysicsCollisionShape* shape;

		// Return the box shape from the cache if it already exists.
		for (unsigned int i = 0; i < shapes_.size(); ++i)
		{
			shape = shapes_[i];
			SC_ASSERT(shape);
			if (shape->type() == PhysicsCollisionShape::kBox)
			{
				btBoxShape* box = static_cast<btBoxShape*>(shape->shape_);
				if (box && box->getHalfExtentsWithMargin() == halfExtents)
				{
					shape->AddRef();
					return shape;
				}
			}
		}

		// Create the box shape and add it to the cache.
		shape = new PhysicsCollisionShape(PhysicsCollisionShape::kBox, new btBoxShape(halfExtents));
		shapes_.push_back(shape);

		return shape;
	}
	PhysicsCollisionShape* PhysicsController::CreateSphere(float radius, const Vector3& scale)
	{
		// Since sphere shapes depend only on the radius, the best we can do is take
		// the largest dimension and apply that as the uniform scale to the rigid body.
		float uniformScale = scale.x;
		if (uniformScale < scale.y)
			uniformScale = scale.y;
		if (uniformScale < scale.z)
			uniformScale = scale.z;

		float scaledRadius = radius * uniformScale;

		PhysicsCollisionShape* shape;

		// Return the sphere shape from the cache if it already exists.
		for (unsigned int i = 0; i < shapes_.size(); ++i)
		{
			shape = shapes_[i];
			SC_ASSERT(shape);
			if (shape->type() == PhysicsCollisionShape::kSphere)
			{
				btSphereShape* sphere = static_cast<btSphereShape*>(shape->shape_);
				if (sphere && sphere->getRadius() == scaledRadius)
				{
					shape->AddRef();
					return shape;
				}
			}
		}

		// Create the sphere shape and add it to the cache.
		shape = new PhysicsCollisionShape(PhysicsCollisionShape::kSphere, new btSphereShape(scaledRadius));
		shapes_.push_back(shape);

		return shape;
	}
	PhysicsCollisionShape* PhysicsController::CreateCapsule(float radius, float height, const Vector3& scale)
	{
		float girthScale = scale.x;
		if (girthScale < scale.z)
			girthScale = scale.z;
		float scaledRadius = radius * girthScale;
		float scaledHeight = height * scale.y - radius * 2.0f;

		PhysicsCollisionShape* shape;

		// Return the capsule shape from the cache if it already exists.
		for (unsigned int i = 0; i < shapes_.size(); i++)
		{
			shape = shapes_[i];
			SC_ASSERT(shape);
			if (shape->type() == PhysicsCollisionShape::kCapsule)
			{
				btCapsuleShape* capsule = static_cast<btCapsuleShape*>(shape->shape_);
				if (capsule && capsule->getRadius() == scaledRadius && capsule->getHalfHeight() == 0.5f * scaledHeight)
				{
					shape->AddRef();
					return shape;
				}
			}
		}

		// Create the capsule shape and add it to the cache.
		shape = new PhysicsCollisionShape(PhysicsCollisionShape::kCapsule, new btCapsuleShape(scaledRadius, scaledHeight));
		shapes_.push_back(shape);

		return shape;
	}
	PhysicsCollisionShape* PhysicsController::CreateMesh(Mesh * mesh, const Vector3& scale, bool dynamic)
	{
		SC_ASSERT(mesh);

		if (!dynamic)
		{
			// Static meshes use btBvhTriangleMeshShape and therefore only support triangle mesh shapes.
			// Dynamic meshes are approximated with a btConvexHullShape (convex wrapper on cloud of vertices)
			// and therefore can support any primitive type.
			if (!mesh->IsTriangleMesh())
			{
				SC_ERROR("Mesh rigid bodies are currently only supported on meshes with TRIANGLES primitive type.");
				return nullptr;
			}
		}

		unsigned int vertex_count = mesh->GetNumberOfVertices();
		if (vertex_count == 0)
		{
			SC_ERROR("Invalid mesh");
			return nullptr;
		}

		// Create mesh data to be populated and store in returned collision shape.
		PhysicsCollisionShape::MeshData * shape_mesh_data = new PhysicsCollisionShape::MeshData();

		// Use enumerator to go through the mesh
		MeshPartsEnumerator mesh_enumerator(mesh);
		MeshPartsEnumerator::PartInfo mesh_part_info;

		// Copy the scaled vertex position data to the rigid body's local buffer.
		Matrix4 scale_matrix;
		Matrix4::CreateScale(scale, &scale_matrix);
		shape_mesh_data->vertex_data = new float[vertex_count * 3];
		Vector3 vertex;
		unsigned int index = 0;
		while (mesh_enumerator.GetNextObject(&mesh_part_info))
		{
			for (unsigned int i = 0; i < mesh_part_info.num_vertices; ++i, ++index)
			{
				vertex.Set(mesh_part_info.vertices[i].position);
				vertex *= scale_matrix;
				memcpy(&(shape_mesh_data->vertex_data[index * 3]), &vertex, sizeof(float) * 3);
			}
		}

		btCollisionShape* collision_shape = nullptr;
		btTriangleIndexVertexArray* mesh_interface = nullptr;

		if (dynamic)
		{
			// For dynamic meshes, use a btConvexHullShape approximation
			btConvexHullShape* original_convex_shape = new btConvexHullShape(shape_mesh_data->vertex_data, vertex_count, sizeof(float)*3);

			// Create a hull approximation for better performance
			btShapeHull* hull = new btShapeHull(original_convex_shape);
			hull->buildHull(original_convex_shape->getMargin());
			collision_shape = new btConvexHullShape((btScalar*)hull->getVertexPointer(), hull->numVertices());

			SC_SAFE_DELETE(hull);
			SC_SAFE_DELETE(original_convex_shape);
		}
		else
		{
			// For static meshes, use btBvhTriangleMeshShape
			mesh_interface = new btTriangleIndexVertexArray();

			const PHY_ScalarType index_type = PHY_INTEGER;
			const int index_stride = sizeof(unsigned int);

			const unsigned char * vertex_base = reinterpret_cast<const unsigned char*>(shape_mesh_data->vertex_data);

			mesh_enumerator.Reset();
			while (mesh_enumerator.GetNextObject(&mesh_part_info))
			{
				// Move the index data into the rigid body's local buffer.
				unsigned int * index_data = new unsigned int[mesh_part_info.num_indices];
				memcpy(index_data, mesh_part_info.indices, sizeof(unsigned int) * mesh_part_info.num_indices);
				
				shape_mesh_data->index_data.push_back(index_data);

				// Create a btIndexedMesh object for the current mesh part.
				btIndexedMesh indexed_mesh;
				indexed_mesh.m_indexType = index_type;
				indexed_mesh.m_numTriangles = mesh_part_info.num_indices / 3; // assume TRIANGLES primitive type
				indexed_mesh.m_triangleIndexBase = reinterpret_cast<const unsigned char*>(index_data);
				indexed_mesh.m_triangleIndexStride = index_stride * 3;
				indexed_mesh.m_numVertices = mesh_part_info.num_vertices;
				indexed_mesh.m_vertexBase = vertex_base;
				indexed_mesh.m_vertexStride = sizeof(float) * 3;
				indexed_mesh.m_vertexType = PHY_FLOAT;

				// Add the indexed mesh data to the mesh interface.
				mesh_interface->addIndexedMesh(indexed_mesh, index_type);

				// Finally move vertex base
				vertex_base += (mesh_part_info.num_vertices * sizeof(float) * 3);
			}

			// Create our collision shape object and store shape_mesh_data in it.
			collision_shape = new btBvhTriangleMeshShape(mesh_interface, true);
		}

		// Create our collision shape object and store shape_mesh_data in it.
		PhysicsCollisionShape * shape = new PhysicsCollisionShape(PhysicsCollisionShape::kMesh, collision_shape, mesh_interface);
		shape->shape_data_.mesh_data = shape_mesh_data;

		shapes_.push_back(shape);

		// We could free the mesh data, but it might be used by another mesh based shape.

		return shape;
	}
	void PhysicsController::DestroyShape(PhysicsCollisionShape* shape)
	{
		if (shape)
		{
			if (shape->GetRefCount() == 1)
			{
				// Remove shape from shape cache.
				std::vector<PhysicsCollisionShape*>::iterator it = std::find(shapes_.begin(), shapes_.end(), shape);
				if (it != shapes_.end())
					shapes_.erase(it);
			}

			// Release the shape.
			shape->Release();
		}
	}
	void PhysicsController::AddConstraint(PhysicsRigidBody * a, PhysicsRigidBody * b, PhysicsConstraint * constraint)
	{
		SC_ASSERT(a);
		SC_ASSERT(constraint);
		SC_ASSERT(world_);

		a->AddConstraint(constraint);
		if (b)
		{
			b->AddConstraint(constraint);
		}
		
		world_->addConstraint(constraint->constraint_);
	}
	bool PhysicsController::CheckConstraintRigidBodies(PhysicsRigidBody * a, PhysicsRigidBody * b)
	{
		SC_ASSERT(a);

		if (!a->SupportsConstraints())
		{
			SC_ASSERT(a->node_);
			SC_ERROR("Rigid body '%s' does not support constraints; unexpected behavior may occur.", a->node_->id());
			return false;
		}
		
		if (b && !b->SupportsConstraints())
		{
			SC_ASSERT(b->node_);
			SC_ERROR("Rigid body '%s' does not support constraints; unexpected behavior may occur.", b->node_->id());
			return false;
		}

		return true;
	}
	void PhysicsController::RemoveConstraint(PhysicsConstraint * constraint)
	{
		SC_ASSERT(constraint);
		SC_ASSERT(world_);

		// Find the constraint and remove it from the physics world.
		for (int i = world_->getNumConstraints() - 1; i >= 0; --i)
		{
			btTypedConstraint * current_constraint = world_->getConstraint(i);
			if (constraint->constraint_ == current_constraint)
			{
				world_->removeConstraint(current_constraint);
				break;
			}
		}
	}

	PhysicsController::HitFilter::HitFilter()
	{
	}
	PhysicsController::HitFilter::~HitFilter()
	{
	}
	bool PhysicsController::HitFilter::Filter(PhysicsCollisionObject * object)
	{
		return false;
	}
	bool PhysicsController::HitFilter::Hit(const PhysicsController::HitResult& result)
	{
		return true;
	}

} // namespace scythe