#include "physics_collision_object.h"

#include "physics_controller.h"

#include "common/sc_assert.h"
#include "common/sc_delete.h"

namespace scythe {

	/**
	 * Internal class used to implement the collidesWith(PhysicsCollisionObject*) function.
	 * @script{ignore}
	 */
	struct CollidesWithCallback : public btCollisionWorld::ContactResultCallback
	{
		/**
		 * Called with each contact. Needed to implement collidesWith(PhysicsCollisionObject*).
		 */
		btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* a, int partIdA, int indexA, const btCollisionObjectWrapper* b, int partIdB, int indexB)
		{
			result = true;
			return 0.0f;
		}

		/**
		 * The result of the callback.
		 */
		bool result;
	};

	PhysicsCollisionObject::PhysicsCollisionObject(Node* node, int group, int mask)
	: node_(node)
	, collision_shape_(nullptr)
	, enabled_(true)
	//, script_listeners_(nullptr)
	, motion_state_(nullptr)
	, group_(group)
	, mask_(mask)
	{
	}
	PhysicsCollisionObject::~PhysicsCollisionObject()
	{
		SC_SAFE_DELETE(motion_state_);

		/*
		if (script_listeners_)
		{
			for (size_t i = 0, count = script_listeners_->size(); i < count; ++i)
			{
				SC_SAFE_DELETE((*script_listeners_)[i]);
			}
			SC_SAFE_DELETE(script_listeners_);
		}
		*/
		PhysicsController * controller = PhysicsController::GetInstance();
		SC_ASSERT(controller);
		controller->DestroyShape(collision_shape_);
	}
	void PhysicsCollisionObject::ClampLinearVelocity(float max_speed)
	{
	}
	void PhysicsCollisionObject::ClampAngularVelocity(float max_speed)
	{
	}
	PhysicsCollisionShape::Type PhysicsCollisionObject::GetShapeType() const
	{
		SC_ASSERT(collision_shape_);
		return collision_shape_->type();
	}
	Node * PhysicsCollisionObject::GetNode() const
	{
		return node_;
	}
	PhysicsCollisionShape * PhysicsCollisionObject::GetCollisionShape() const
	{
		return collision_shape_;
	}
	bool PhysicsCollisionObject::IsKinematic() const
	{
		switch (type())
		{
		case kGhostObject:
		case kCharacter:
			return true;
		default:
			SC_ASSERT(GetCollisionObject());
			return GetCollisionObject()->isKinematicObject();
		}
	}

	bool PhysicsCollisionObject::IsStatic() const
	{
		switch (type())
		{
		case kGhostObject:
		case kCharacter:
			return false;
		default:
			SC_ASSERT(GetCollisionObject());
			return GetCollisionObject()->isStaticObject();
		}
	}

	bool PhysicsCollisionObject::IsDynamic() const
	{
		SC_ASSERT(GetCollisionObject());
		return !GetCollisionObject()->isStaticOrKinematicObject();
	}

	bool PhysicsCollisionObject::IsEnabled() const
	{
		return enabled_;
	}

	void PhysicsCollisionObject::SetEnabled(bool enable)
	{
		if (enable)
		{  
			if (!enabled_)
			{
				PhysicsController * controller = PhysicsController::GetInstance();
				controller->AddCollisionObject(this);
				motion_state_->UpdateTransformFromNode();
				enabled_ = true;
			}
		}
		else
		{
			if (enabled_)
			{
				PhysicsController * controller = PhysicsController::GetInstance();
				controller->RemoveCollisionObject(this, false);
				enabled_ = false;
			}
		}
	}

	void PhysicsCollisionObject::AddCollisionListener(CollisionListener* listener, PhysicsCollisionObject* object, bool duplicate)
	{
		PhysicsController * controller = PhysicsController::GetInstance();
		SC_ASSERT(controller);
		controller->AddCollisionListener(listener, this, object, duplicate);
	}

	void PhysicsCollisionObject::RemoveCollisionListener(CollisionListener* listener, PhysicsCollisionObject* object)
	{
		PhysicsController * controller = PhysicsController::GetInstance();
		SC_ASSERT(controller);
		controller->RemoveCollisionListener(listener, this, object);
	}
	void PhysicsCollisionObject::AddCollisionListener(const char* function, PhysicsCollisionObject* object)
	{
		/*
		ScriptListener* listener = ScriptListener::create(function);
		if (!listener)
			return; // falied to load

		if (!script_listeners_)
			script_listeners_ = new std::vector<ScriptListener*>();
		script_listeners_->push_back(listener);

		AddCollisionListener(listener, object);
		*/
	}
	void PhysicsCollisionObject::RemoveCollisionListener(const char* function, PhysicsCollisionObject* object)
	{
		/*
		if (!script_listeners_)
			return;

		std::string url = function;
		for (size_t i = 0, count = script_listeners_->size(); i < count; ++i)
		{
			if ((*script_listeners_)[i]->url == url)
			{
				RemoveCollisionListener((*script_listeners_)[i], object);
				SC_SAFE_DELETE((*script_listeners_)[i]);
				script_listeners_->erase(script_listeners_->begin() + i);
				return;
			}
		}
		*/
	}
	bool PhysicsCollisionObject::CollidesWith(PhysicsCollisionObject* object) const
	{
		PhysicsController * controller = PhysicsController::GetInstance();
		SC_ASSERT(controller && controller->world_);
		SC_ASSERT(object && object->GetCollisionObject());
		SC_ASSERT(GetCollisionObject());

		static CollidesWithCallback callback;

		callback.result = false;
		controller->world_->contactPairTest(GetCollisionObject(), object->GetCollisionObject(), callback);
		return callback.result;
	}

	PhysicsCollisionObject::CollisionPair::CollisionPair(PhysicsCollisionObject* objectA, PhysicsCollisionObject* objectB)
	: objectA(objectA)
	, objectB(objectB)
	{
		// unused
	}
	bool PhysicsCollisionObject::CollisionPair::operator < (const CollisionPair& collisionPair) const
	{
		// If the pairs are equal, then return false.
		if ((objectA == collisionPair.objectA && objectB == collisionPair.objectB) || (objectA == collisionPair.objectB && objectB == collisionPair.objectA))
			return false;

		// We choose to compare based on objectA arbitrarily.
		if (objectA < collisionPair.objectA)
			return true;

		if (objectA == collisionPair.objectA)
			return objectB < collisionPair.objectB;

		return false;
	}

	PhysicsCollisionObject::PhysicsMotionState::PhysicsMotionState(Node* node, PhysicsCollisionObject* collisionObject, const Vector3* centerOfMassOffset)
	: node_(node)
	, collision_object_(collisionObject)
	, center_of_mass_offset_(btTransform::getIdentity())
	{
		if (centerOfMassOffset)
		{
			// Store the center of mass offset.
			center_of_mass_offset_.setOrigin(BV(*centerOfMassOffset));
		}

		UpdateTransformFromNode();
	}
	PhysicsCollisionObject::PhysicsMotionState::~PhysicsMotionState()
	{
	}
	void PhysicsCollisionObject::PhysicsMotionState::getWorldTransform(btTransform &transform) const
	{
		SC_ASSERT(node_);
		SC_ASSERT(collision_object_);

		if (collision_object_->IsKinematic())
			UpdateTransformFromNode();

		transform = center_of_mass_offset_.inverse() * world_transform_;
	}
	void PhysicsCollisionObject::PhysicsMotionState::setWorldTransform(const btTransform &transform)
	{
		SC_ASSERT(node_);

		world_transform_ = transform * center_of_mass_offset_;
			
		const btQuaternion& rot = world_transform_.getRotation();
		const btVector3& pos = world_transform_.getOrigin();

		node_->SetRotation(rot.x(), rot.y(), rot.z(), rot.w());
		node_->SetTranslation(pos.x(), pos.y(), pos.z());
	}
	void PhysicsCollisionObject::PhysicsMotionState::UpdateTransformFromNode() const
	{
		SC_ASSERT(node_);

		// Store the initial world transform (minus the scale) for use by Bullet later on.
		Quaternion rotation;
		const Matrix4& m = node_->GetWorldMatrix();
		m.GetRotation(&rotation);

		if (!center_of_mass_offset_.getOrigin().isZero())
		{
			// When there is a center of mass offset, we modify the initial world transformation
			// so that when physics is initially applied, the object is in the correct location.
			btTransform offset = btTransform(BQ(rotation), btVector3(0.0f, 0.0f, 0.0f)) * center_of_mass_offset_.inverse();

			btVector3 origin(m.m[12] + center_of_mass_offset_.getOrigin().getX() + offset.getOrigin().getX(), 
							 m.m[13] + center_of_mass_offset_.getOrigin().getY() + offset.getOrigin().getY(), 
							 m.m[14] + center_of_mass_offset_.getOrigin().getZ() + offset.getOrigin().getZ());
			world_transform_ = btTransform(BQ(rotation), origin);
		}
		else
		{
			world_transform_ = btTransform(BQ(rotation), btVector3(m.m[12], m.m[13], m.m[14]));
		}
	}
	void PhysicsCollisionObject::PhysicsMotionState::SetCenterOfMassOffset(const Vector3& centerOfMassOffset)
	{
		center_of_mass_offset_.setOrigin(BV(centerOfMassOffset));
	}

} // namespace scythe