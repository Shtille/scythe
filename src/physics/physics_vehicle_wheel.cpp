#include "physics_vehicle_wheel.h"
#include "physics_vehicle.h"

#include "common/sc_assert.h"
#include "common/log.h"

#include "node.h"

namespace scythe {

	PhysicsVehicleWheel::PhysicsVehicleWheel(Node* node, const PhysicsCollisionShape::Definition& shape, const PhysicsRigidBody::Parameters& parameters)
	: PhysicsCollisionObject(node)
	, host_(nullptr)
	, index_in_host_(0)
	{
		FindAncestorAndBind();
	}

	PhysicsVehicleWheel::PhysicsVehicleWheel(Node* node)
	: PhysicsCollisionObject(node)
	, host_(nullptr)
	, index_in_host_(0)
	{
		FindAncestorAndBind();
	}

	PhysicsVehicleWheel::~PhysicsVehicleWheel()
	{
	}

	btCollisionObject* PhysicsVehicleWheel::GetCollisionObject() const
	{
		SC_ASSERT(host_);

		return host_->GetCollisionObject();
	}

	PhysicsCollisionObject::Type PhysicsVehicleWheel::type() const
	{
		return PhysicsCollisionObject::kVehicleWheel;
	}

	void PhysicsVehicleWheel::SetEnabled(bool enable)
	{
		SC_ERROR("Operation not supported (PhysicsVehicleWheel::SetEnabled(bool)). Use host vehicle instead.");
	}

	void PhysicsVehicleWheel::FindAncestorAndBind()
	{
		SC_ASSERT(GetNode());

		// Search for the first PhysicsVehicle that shares a common ancestor, and
		// bind with it. The following code performs a naive search; nothing more
		// sophisticated is deemed necessary because:
		// (a) The root of the scene is NOT a node
		// (b) Scene graphs tend to be relatively flat.
		//
		// The search ends when a vehicle is found or n is null:
		// 1: Let n = this node
		// 2: Visit each sibling of n and perform a breadth-first search of its descendants
		// 3: Let n = the parent of n
		// 4: Go to 2.
		PhysicsVehicle* host = nullptr;
		Node* m;
		for (Node* n = GetNode(); n && !host; n = n->GetParent())
		{
			// Visit previous siblings starting with n
			for (m = n; m && !host; m = m->GetPreviousSibling())
			{
				host = FindVehicle(m);
			}

			// Visit siblings after n
			for (m = n->GetNextSibling(); m && !host; m = m->GetNextSibling())
			{
				host = FindVehicle(m);
			}
		}

		// Note: Currently this method is silent on failure to find a host.
		if (host)
		{
			host->AddWheel(this);
			initial_offset_ = node_->GetTranslation() - host->node_->GetTranslation();
		}
	}

	PhysicsVehicle* PhysicsVehicleWheel::FindVehicle(Node* node)
	{
		PhysicsCollisionObject* collisionObject = node->GetCollisionObject();
		if (collisionObject && collisionObject->type() == PhysicsCollisionObject::kVehicle)
		{
			return static_cast<PhysicsVehicle*>(collisionObject);
		}

		PhysicsVehicle* result = nullptr;
		for (Node* p = node->GetFirstChild(); p && !result; p = p->GetNextSibling())
		{
			result = FindVehicle(p);
		}
		return result;
	}

	void PhysicsVehicleWheel::SetHost(PhysicsVehicle* host, unsigned int indexInHost)
	{
		host_ = host;
		index_in_host_ = indexInHost;
	}

	void PhysicsVehicleWheel::AddToVehicle(btRaycastVehicle* vehicle)
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->GetWheelCount() == vehicle->getNumWheels() + 1);

		// Use safe defaults for now. Properties are assigned elsewhere.
		btRaycastVehicle::btVehicleTuning tuning;
		vehicle->addWheel(
			btVector3(0, 0, 0),
			btVector3(0, -1, 0),
			btVector3(-1, 0, 0),
			0.6f,
			0.5f,
			tuning,
			false);
	}

	void PhysicsVehicleWheel::Transform(Node* node) const
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->node_);

		node->SetRotation(orientation_);

		// Use only the component parallel to the defined strut line
		Vector3 strutLine;
		GetWheelDirection(&strutLine);
		host_->node_->GetMatrix().TransformVector(&strutLine);
		Vector3 wheelPos;
		GetWheelPos(&wheelPos);
		node->SetTranslation(wheelPos + strutLine*(strutLine.Dot(position_delta_) / strutLine.LengthSquared()));
	}

	void PhysicsVehicleWheel::Update(float elapsedTime)
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		const btTransform& trans = host_->vehicle_->getWheelInfo(index_in_host_).m_worldTransform;
		const btQuaternion& rot = trans.getRotation();
		const btVector3& pos = trans.getOrigin();
		orientation_.Set(rot.x(), rot.y(), rot.z(), rot.w());

		Vector3 commandedPosition(pos.x(), pos.y(), pos.z());
		Vector3 wheelPos;
		GetWheelPos(&wheelPos);
		commandedPosition -= wheelPos;

		// Filter out noise from Bullet
		Vector3 delta(position_delta_, commandedPosition);
		float threshold = GetStrutRestLength() * 2.0f;
		float responseTime = (delta.LengthSquared() > threshold*threshold) ? 0.f : 60.f;
		position_delta_.Smooth(commandedPosition, elapsedTime, responseTime);
	}

	void PhysicsVehicleWheel::GetConnectionDefault(Vector3* result) const
	{
		// projected strut length
		GetWheelDirection(result);
		result->Normalize();
		float length = 0.58f * GetStrutRestLength();
		*result *= -length;

		// nudge wheel contact point to outer edge of tire for stability
		Vector3 nudge;
		GetWheelAxle(&nudge);
		nudge *= nudge.Dot(initial_offset_);
		nudge.Normalize();
		*result += nudge * 0.068f * GetWheelRadius(); // rough-in for tire width

		// offset at bind time
		*result += initial_offset_;
	}

	void PhysicsVehicleWheel::GetWheelPos(Vector3* result) const
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->node_);

		*result = initial_offset_;
		host_->node_->GetMatrix().TransformPoint(result);
	}

	bool PhysicsVehicleWheel::IsSteerable() const
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		return host_->vehicle_->getWheelInfo(index_in_host_).m_bIsFrontWheel;
	}

	void PhysicsVehicleWheel::SetSteerable(bool steerable)
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		host_->vehicle_->getWheelInfo(index_in_host_).m_bIsFrontWheel = steerable;
	}

	void PhysicsVehicleWheel::GetWheelDirection(Vector3* wheelDirection) const
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		const btVector3& v = host_->vehicle_->getWheelInfo(index_in_host_).m_wheelDirectionCS;
		wheelDirection->Set(v.x(), v.y(), v.z());
	}

	void PhysicsVehicleWheel::SetWheelDirection(const Vector3& wheelDirection)
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		host_->vehicle_->getWheelInfo(index_in_host_).m_wheelDirectionCS.setValue(wheelDirection.x, wheelDirection.y, wheelDirection.z);
	}

	void PhysicsVehicleWheel::GetWheelAxle(Vector3* wheelAxle) const
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		const btVector3& v = host_->vehicle_->getWheelInfo(index_in_host_).m_wheelAxleCS;
		wheelAxle->Set(v.x(), v.y(), v.z());
	}

	void PhysicsVehicleWheel::SetWheelAxle(const Vector3& wheelAxle)
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		host_->vehicle_->getWheelInfo(index_in_host_).m_wheelAxleCS.setValue(wheelAxle.x, wheelAxle.y, wheelAxle.z);
	}

	void PhysicsVehicleWheel::GetStrutConnectionOffset(Vector3* strutConnectionOffset) const
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		const btVector3& v = host_->vehicle_->getWheelInfo(index_in_host_).m_chassisConnectionPointCS;
		strutConnectionOffset->Set(v.x(), v.y(), v.z());
		Vector3 strutConnectionDefault;
		GetConnectionDefault(&strutConnectionDefault);
		*strutConnectionOffset -= strutConnectionDefault;
	}

	void PhysicsVehicleWheel::SetStrutConnectionOffset(const Vector3& strutConnectionOffset)
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		Vector3 strutConnectionPoint;
		GetConnectionDefault(&strutConnectionPoint);
		strutConnectionPoint += strutConnectionOffset;
		host_->vehicle_->getWheelInfo(index_in_host_).m_chassisConnectionPointCS.setValue(strutConnectionPoint.x,
																						strutConnectionPoint.y,
																						strutConnectionPoint.z);
	}

	float PhysicsVehicleWheel::GetStrutRestLength() const
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		return host_->vehicle_->getWheelInfo(index_in_host_).m_suspensionRestLength1;
	}

	void PhysicsVehicleWheel::SetStrutRestLength(float strutRestLength)
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		host_->vehicle_->getWheelInfo(index_in_host_).m_suspensionRestLength1 = strutRestLength;
	}

	float PhysicsVehicleWheel::GetStrutTravelMax() const
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		return host_->vehicle_->getWheelInfo(index_in_host_).m_maxSuspensionTravelCm / 100.0f;
	}

	void PhysicsVehicleWheel::SetStrutTravelMax(float strutTravelMax)
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		host_->vehicle_->getWheelInfo(index_in_host_).m_maxSuspensionTravelCm = strutTravelMax * 100.0f;
	}

	float PhysicsVehicleWheel::GetStrutStiffness() const
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		return host_->vehicle_->getWheelInfo(index_in_host_).m_suspensionStiffness;
	}

	void PhysicsVehicleWheel::SetStrutStiffness(float strutStiffness)
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		host_->vehicle_->getWheelInfo(index_in_host_).m_suspensionStiffness = strutStiffness;
	}

	float PhysicsVehicleWheel::GetStrutDampingCompression() const
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		return host_->vehicle_->getWheelInfo(index_in_host_).m_wheelsDampingCompression;
	}

	void PhysicsVehicleWheel::SetStrutDampingCompression(float strutDampingCompression)
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		host_->vehicle_->getWheelInfo(index_in_host_).m_wheelsDampingCompression = strutDampingCompression;
	}

	float PhysicsVehicleWheel::GetStrutDampingRelaxation() const
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		return host_->vehicle_->getWheelInfo(index_in_host_).m_wheelsDampingRelaxation;
	}

	void PhysicsVehicleWheel::SetStrutDampingRelaxation(float strutDampingRelaxation)
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		host_->vehicle_->getWheelInfo(index_in_host_).m_wheelsDampingRelaxation = strutDampingRelaxation;
	}

	float PhysicsVehicleWheel::GetStrutForceMax() const
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		return host_->vehicle_->getWheelInfo(index_in_host_).m_maxSuspensionForce;
	}

	void PhysicsVehicleWheel::SetStrutForceMax(float strutForceMax)
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		host_->vehicle_->getWheelInfo(index_in_host_).m_maxSuspensionForce = strutForceMax;
	}

	float PhysicsVehicleWheel::GetFrictionBreakout() const
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		return host_->vehicle_->getWheelInfo(index_in_host_).m_frictionSlip;
	}

	void PhysicsVehicleWheel::SetFrictionBreakout(float frictionBreakout)
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		host_->vehicle_->getWheelInfo(index_in_host_).m_frictionSlip = frictionBreakout;
	}

	float PhysicsVehicleWheel::GetWheelRadius() const
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		return host_->vehicle_->getWheelInfo(index_in_host_).m_wheelsRadius;
	}

	void PhysicsVehicleWheel::SetWheelRadius(float wheelRadius)
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		host_->vehicle_->getWheelInfo(index_in_host_).m_wheelsRadius = wheelRadius;
	}

	float PhysicsVehicleWheel::GetRollInfluence() const
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		return host_->vehicle_->getWheelInfo(index_in_host_).m_rollInfluence;
	}

	void PhysicsVehicleWheel::SetRollInfluence(float rollInfluence)
	{
		SC_ASSERT(host_);
		SC_ASSERT(host_->vehicle_);

		host_->vehicle_->getWheelInfo(index_in_host_).m_rollInfluence = rollInfluence;
	}

} // namespace scythe