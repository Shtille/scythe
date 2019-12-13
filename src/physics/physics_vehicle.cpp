#include "physics_vehicle.h"
#include "physics_vehicle_wheel.h"
#include "physics_controller.h"

#include "common/sc_assert.h"
#include "common/sc_delete.h"
#include "math/common_math.h"

#include <cmath>

namespace scythe {

	static inline void Smooth(float* x, float target, float elapsedTime, float riseTime, float fallTime)
	{
		SC_ASSERT(x);
		
		if (elapsedTime > 0.0f)
		{
			float delta = target - *x;
			*x += delta * elapsedTime / (elapsedTime + (delta > 0.0f ? riseTime : fallTime));
		}
	}

	/**
	  * The default vehicle raycaster in Bullet currently does not filter out the vehicle's own
	  * rigid body from the ray test which can result in unexpected behavior. These implementations
	  * are intended to fix that.
	  *
	  * @script{ignore}
	  */
	class ClosestNotMeRayResultCallback : public btCollisionWorld::ClosestRayResultCallback
	{
	public:

		ClosestNotMeRayResultCallback(const btVector3& from, const btVector3& to, btCollisionObject* me)
		: btCollisionWorld::ClosestRayResultCallback(from, to)
		, me_(me)
		{
		}

		btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
		{
			if (rayResult.m_collisionObject == me_)
				return 1.0f;

			return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
		}

	private:

		btCollisionObject * me_;
	};

	/**
	  * @script{ignore}
	  */
	class VehicleNotMeRaycaster : public btVehicleRaycaster
	{
	public:

		VehicleNotMeRaycaster(btDynamicsWorld* world, btCollisionObject* me)
		: dynamics_world_(world)
		, me_(me)
		{
		}

		void* castRay(const btVector3& from, const btVector3& to, btVehicleRaycasterResult& result)
		{
			ClosestNotMeRayResultCallback rayCallback(from, to, me_);
			dynamics_world_->rayTest(from, to, rayCallback);

			if (rayCallback.hasHit())
			{
				const btRigidBody* body = btRigidBody::upcast(rayCallback.m_collisionObject);
				if (body && body->hasContactResponse())
				{
					result.m_hitPointInWorld = rayCallback.m_hitPointWorld;
					result.m_hitNormalInWorld = rayCallback.m_hitNormalWorld;
					result.m_hitNormalInWorld.normalize();
					result.m_distFraction = rayCallback.m_closestHitFraction;
					return (void*)body;
				}
			}
			return (void*)0;
		}

	private:

		btDynamicsWorld * dynamics_world_;
		btCollisionObject * me_;
	};

	PhysicsVehicle::PhysicsVehicle(Node* node, const PhysicsCollisionShape::Definition& shape, const PhysicsRigidBody::Parameters& parameters)
	: PhysicsCollisionObject(node)
	, speed_smoothed_(0.0f)
	{
		// Note that the constructor for PhysicsRigidBody calls addCollisionObject and so
		// that is where the rigid body gets added to the dynamics world.
		rigid_body_ = new PhysicsRigidBody(node, shape, parameters);

		Initialize();
	}

	PhysicsVehicle::PhysicsVehicle(Node* node, PhysicsRigidBody* rigidBody)
	: PhysicsCollisionObject(node)
	, speed_smoothed_(0.0f)
	{
		rigid_body_ = rigidBody;

		Initialize();
	}

	void PhysicsVehicle::Initialize()
	{
		SC_ASSERT(GetNode());

		// Safe default values
		SetSteeringGain(0.5f);
		SetBrakingForce(350.0f);
		SetDrivingForce(2000.0f);
		SetSteerdown(0.f, 1.f);
		SetBrakedown(1000.f, 0.f);
		SetDrivedown(1000.f, 0.f);
		SetBoost(0.f, 1.f);
		SetDownforce(0.f);

		PhysicsController * controller = PhysicsController::GetInstance();
		SC_ASSERT(controller);

		// Create the vehicle and add it to world
		btRigidBody * body = static_cast<btRigidBody*>(rigid_body_->GetCollisionObject());
		btDynamicsWorld * dynamicsWorld = controller->world_;
		vehicle_raycaster_ = new VehicleNotMeRaycaster(dynamicsWorld, body);
		vehicle_ = new btRaycastVehicle(vehicle_tuning_, body, vehicle_raycaster_);
		body->setActivationState(DISABLE_DEACTIVATION);
		dynamicsWorld->addVehicle(vehicle_);
		vehicle_->setCoordinateSystem(0, 1, 2);
	}

	PhysicsVehicle::~PhysicsVehicle()
	{
		// Note that the destructor for PhysicsRigidBody calls removeCollisionObject and so
		// that is where the rigid body gets removed from the dynamics world. The vehicle
		// itself is just an action interface in the dynamics world.
		SC_SAFE_DELETE(vehicle_);
		SC_SAFE_DELETE(vehicle_raycaster_);
		SC_SAFE_DELETE(rigid_body_);
	}

	btCollisionObject* PhysicsVehicle::GetCollisionObject() const
	{
		SC_ASSERT(rigid_body_);

		return rigid_body_->GetCollisionObject();
	}

	PhysicsCollisionObject::Type PhysicsVehicle::type() const
	{
		return PhysicsCollisionObject::kVehicle;
	}

	PhysicsRigidBody * PhysicsVehicle::GetRigidBody() const
	{
		SC_ASSERT(rigid_body_);

		return rigid_body_;
	}

	void PhysicsVehicle::SetEnabled(bool enable)
	{
		GetRigidBody()->SetEnabled(enable);
	}

	unsigned int PhysicsVehicle::GetWheelCount() const
	{
		return (unsigned int)wheels_.size();
	}

	PhysicsVehicleWheel* PhysicsVehicle::GetWheel(unsigned int index)
	{
		return wheels_.at(index);
	}

	void PhysicsVehicle::AddWheel(PhysicsVehicleWheel * wheel)
	{
		unsigned int i = (unsigned int)wheels_.size();
		wheels_.push_back(wheel);
		wheel->SetHost(this, i);
		wheel->AddToVehicle(vehicle_);
	}

	float PhysicsVehicle::GetSpeedKph() const
	{
		return vehicle_->getCurrentSpeedKmHour();
	}

	float PhysicsVehicle::GetSpeedSmoothKph() const
	{
		return speed_smoothed_;
	}

	void PhysicsVehicle::Update(float elapsedTime, float steering, float braking, float driving)
	{
		float v = GetSpeedKph();
		Smooth(&speed_smoothed_, v, elapsedTime, 0.0f, 1.2f);
		if (elapsedTime > 0.0f)
		{
			// Avoid accumulation of downforce while paused (zero elapsedTime)
			ApplyDownforce();
		}

		// Adjust control inputs based on vehicle speed.
		steering = GetSteering(v, steering);
		driving = GetDriving(v, driving, braking);
		braking = GetBraking(v, braking);

		// Allow braking to take precedence over driving.
		if (driving > 0.0f && braking > 0.0f)
		{
			driving = 0.0f;
		}

		PhysicsVehicleWheel* wheel;
		for (int i = 0; i < vehicle_->getNumWheels(); ++i)
		{
			wheel = GetWheel(i);

			if (wheel->IsSteerable())
			{
				vehicle_->setSteeringValue(steering * steering_gain_, i);
			}
			else
			{
				vehicle_->applyEngineForce(driving * driving_force_, i);
				vehicle_->setBrake(braking * braking_force_, i);
			}

			wheel->Update(elapsedTime);
			wheel->Transform(wheel->GetNode());
		}
	}

	void PhysicsVehicle::Reset()
	{
		rigid_body_->SetLinearVelocity(Vector3::Zero());
		rigid_body_->SetAngularVelocity(Vector3::Zero());
		speed_smoothed_ = 0.0f;
	}

	float PhysicsVehicle::GetSteering(float v, float rawSteering) const
	{
		float gain = 1.0f;
		if (steerdown_speed_ > kFloatSmall)
		{
			gain = Max(steerdown_gain_, 1.0f - (1.0f - steerdown_gain_) * fabs(v) / steerdown_speed_);
		}

		return rawSteering * gain;
	}

	float PhysicsVehicle::GetBraking(float v, float rawBraking) const
	{
		float reduc = 0.0f;
		float delta = brakedown_full_ - brakedown_start_;
		if (delta > kFloatSmall)
		{
			reduc = Max(0.0f, (v - brakedown_start_) / delta);
			reduc *= reduc;
		}

		return Max(0.0f, rawBraking - reduc);
	}

	float PhysicsVehicle::GetDriving(float v, float rawDriving, float rawBraking) const
	{
		float reduc = 0.0f;
		float delta = drivedown_full_ - drivedown_start_;
		if (rawBraking == 0.0f && delta > kFloatSmall)
		{
			reduc = Max(0.0f, (v - drivedown_start_) / delta);
			reduc *= reduc;
		}

		float gain = 1.0f;
		if (boost_speed_ > kFloatSmall)
		{
			gain = Max(1.0f, boost_gain_ - (boost_gain_ - 1.0f) * fabs(v) / boost_speed_);
		}

		return gain * rawDriving - reduc;
	}

	void PhysicsVehicle::ApplyDownforce()
	{
		const float kAirDensity = 1.2f;
		const float kKphToMps = 1.0f / 3.6f;

		float v = speed_smoothed_ * kKphToMps;

		// Dynamic pressure
		float q = 0.5f * kAirDensity * v * v;

		// downforce_ is the product of reference area and the aerodynamic coefficient
		rigid_body_->ApplyForce(Vector3(0.0f, -downforce_ * q, 0.0f));
	}

	float PhysicsVehicle::GetSteeringGain() const
	{
		return steering_gain_;
	}

	void PhysicsVehicle::SetSteeringGain(float steeringGain)
	{
		steering_gain_ = steeringGain;
	}

	float PhysicsVehicle::GetBrakingForce() const
	{
		return braking_force_;
	}

	void PhysicsVehicle::SetBrakingForce(float brakingForce)
	{
		braking_force_ = brakingForce;
	}

	float PhysicsVehicle::GetDrivingForce() const
	{
		return driving_force_;
	}

	void PhysicsVehicle::SetDrivingForce(float drivingForce)
	{
		driving_force_ = drivingForce;
	}

	float PhysicsVehicle::GetSteerdownSpeed() const
	{
		return steerdown_speed_;
	}

	float PhysicsVehicle::GetSteerdownGain() const
	{
		return steerdown_gain_;
	}

	void PhysicsVehicle::SetSteerdown(float steerdownSpeed, float steerdownGain)
	{
		steerdown_speed_ = steerdownSpeed;
		steerdown_gain_ = steerdownGain;
	}

	float PhysicsVehicle::GetBrakedownStart() const
	{
		return brakedown_start_;
	}

	float PhysicsVehicle::GetBrakedownFull() const
	{
		return brakedown_full_;
	}

	void PhysicsVehicle::SetBrakedown(float brakedownStart, float brakedownFull)
	{
		brakedown_start_ = brakedownStart;
		brakedown_full_ = brakedownFull;
	}

	float PhysicsVehicle::GetDrivedownStart() const
	{
		return drivedown_start_;
	}

	float PhysicsVehicle::GetDrivedownFull() const
	{
		return drivedown_full_;
	}

	void PhysicsVehicle::SetDrivedown(float drivedownStart, float drivedownFull)
	{
		drivedown_start_ = drivedownStart;
		drivedown_full_ = drivedownFull;
	}

	float PhysicsVehicle::GetBoostSpeed() const
	{
		return boost_speed_;
	}

	float PhysicsVehicle::GetBoostGain() const
	{
		return boost_gain_;
	}

	void PhysicsVehicle::SetBoost(float boostSpeed, float boostGain)
	{
		boost_speed_ = boostSpeed;
		boost_gain_ = boostGain;
	}

	float PhysicsVehicle::GetDownforce() const
	{
		return downforce_;
	}

	void PhysicsVehicle::SetDownforce(float downforce)
	{
		downforce_ = downforce;
	}

} // namespace scythe