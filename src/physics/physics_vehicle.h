#ifndef __SCYTHE_PHYSICS_VEHICLE_H__
#define __SCYTHE_PHYSICS_VEHICLE_H__

#include "physics_collision_object.h"
#include "physics_rigid_body.h"

#include <vector>

namespace scythe {

	/**
	 * Defines a class for vehicle physics.
	 *
	 * In addition to its own properties defined below, a vehicle has available
	 * to it all of the properties of a rigid body such as shape, mass, friction,
	 * etc which correspond to the vehicle body:
	 */
	class PhysicsVehicle : public PhysicsCollisionObject
	{
		friend class Node;
		friend class PhysicsVehicleWheel;

	public:

		/**
		 * @see PhysicsCollisionObject#type
		 */
		PhysicsCollisionObject::Type type() const;

		/**
		 * Returns the rigid body associated with this vehicle.
		 */
		PhysicsRigidBody * GetRigidBody() const;

		/**
		 * Sets whether the associated rigid body is enabled or disabled
		 * in the physics world.
		 *
		 * @param enable true enables the collision object, false disables it.
		 */
		void SetEnabled(bool enable);

		/**
		 * Gets the number of wheels on this vehicle.
		 *
		 * @return the number of wheels on this vehicle.
		 */
		unsigned int GetWheelCount() const;

		/**
		 * Gets the wheel at the specified index.
		 * 
		 * @param i index of wheel.
		 * @return the wheel at the specified index.
		 */
		PhysicsVehicleWheel* GetWheel(unsigned int i);

		/**
		 * Permanently adds a wheel to this vehicle.
		 *
		 * @param wheel the wheel to add.
		 */
		void AddWheel(PhysicsVehicleWheel* wheel);

		/**
		 * Gets an indication of vehicle speed in kilometers per hour.
		 */
		float GetSpeedKph() const;

		/**
		 * Gets a lagged version of vehicle speed in kilometers per hour,
		 * for example that might be used to control engine sounds.
		 */
		float GetSpeedSmoothKph() const;

		/**
		 * Updates the vehicle state using the specified normalized command
		 * inputs, and updates the transform on the visual node for each wheel.
		 *
		 * @param elapsedTime The elapsed game time.
		 * @param steering steering command (-1 to 1).
		 * @param braking braking command (0 to 1).
		 * @param driving net drivetrain command (0 to 1).
		 */
		void Update(float elapsedTime, float steering, float braking, float driving);

		/**
		 * Resets the vehicle's state, for example in preparation for a reposition.
		 */
		void Reset();

		/**
		 * Gets steering gain at full deflection.
		 *
		 * @return steering gain at full deflection.
		 */
		float GetSteeringGain() const;

		/**
		 * Sets steering gain at full deflection.
		 *
		 * @param steeringGain steering gain at full deflection.
		 */
		void SetSteeringGain(float steeringGain);

		/**
		 * Gets braking force at full braking.
		 *
		 * @return braking force at full braking.
		 */
		float GetBrakingForce() const;

		/**
		 * Sets braking force at full braking.
		 *
		 * @param brakingForce braking force at full braking.
		 */
		void SetBrakingForce(float brakingForce);

		/**
		 * Gets driving force at full throttle.
		 *
		 * @return driving force at full throttle.
		 */
		float GetDrivingForce() const;

		/**
		 * Sets driving force at full throttle.
		 *
		 * @param drivingForce driving force at full throttle.
		 */
		void SetDrivingForce(float drivingForce);

		/**
		 * Returns speed at the point of reduced steering, in km/h.
		 * A point of reduced steering is defined by speed and gain.
		 * Steering authority will reduce linearly with speed up to
		 * this point, and remain constant above that.
		 *
		 * @return speed at the point of reduced steering, in km/h.
		 */
		float GetSteerdownSpeed() const;

		/**
		 * Returns gain at the point of reduced steering, typically
		 * less than 1.
		 * A point of reduced steering is defined by speed and gain.
		 * Steering authority will reduce linearly with speed up to
		 * this point, and remain constant above that.
		 *
		 * @return gain at the point of reduced steering.
		 */
		float GetSteerdownGain() const;

		/**
		 * Sets the point of reduced steering, defined by speed and
		 * gain. Typically the gain value is less than 1.
		 * Steering authority will reduce linearly with speed up to
		 * this point, and remain constant above that.
		 *
		 * @param steerdownSpeed speed at the point of reduced steering,
		 *     in km/h.
		 * @param steerdownGain gain at the point of reduced steering.
		 *     A gain of 1 will effectively disable the feature.
		 */
		void SetSteerdown(float steerdownSpeed, float steerdownGain);

		/**
		 * Returns speed where braking starts to fade, in km/h.
		 *
		 * @return speed where braking starts to fade, in km/h.
		 */
		float GetBrakedownStart() const;

		/**
		 * Returns speed where braking is fully faded, in km/h.
		 * This speed is typically greater than the brakedownStart
		 * speed.
		 *
		 * @return speed where braking is fully faded, in km/h.
		 */
		float GetBrakedownFull() const;

		/**
		 * Sets points that control fade of brake force with speed,
		 * in km/h.
		 *
		 * @param brakedownStart braking fades above this speed.
		 *     A very large value will effectively disable the feature.
		 * @param brakedownFull braking is fully faded at this speed.
		 *     This speed is typically greater than the brakedownStart
		 *     speed.
		 */
		void SetBrakedown(float brakedownStart, float brakedownFull);

		/**
		 * Returns speed where driving force starts to fade, in km/h.
		 *
		 * @return speed where driving force starts to fade, in km/h.
		 */
		float GetDrivedownStart() const;

		/**
		 * Returns speed where driving force is fully faded, in km/h.
		 * This speed is typically greater than the drivedownStart
		 * speed.
		 *
		 * @return speed where driving force is fully faded, in km/h.
		 */
		float GetDrivedownFull() const;

		/**
		 * Sets points that control fade of driving force with speed,
		 * in km/h.
		 *
		 * @param drivedownStart driving force fades above this speed.
		 *     A very large value will effectively disable the feature.
		 * @param drivedownFull driving force is fully faded at this speed.
		 *     This speed is typically greater than the drivedownStart
		 *     speed.
		 */
		void SetDrivedown(float drivedownStart, float drivedownFull);

		/**
		 * Returns upper limit of low-speed boost effect, in km/h.
		 * Driving force is boosted by a specified factor at zero speed,
		 * and that factor fades linearly with speed reaching 1 at
		 * this speed.
		 *
		 * @return upper limit of low-speed boost effect, in km/h.
		 */
		float GetBoostSpeed() const;

		/**
		 * Returns boost gain at zero speed, typically greater than 1.
		 * Driving force is boosted by this factor at zero speed, and
		 * that factor fades linearly with speed reaching 1 at a
		 * specified speed.
		 *
		 * @return boost gain at zero speed.
		 */
		float GetBoostGain() const;

		/**
		 * Sets parameters that define low-speed boost of the driving force.
		 * Driving force is boosted by the specified factor at zero speed,
		 * and that factor fades linearly with speed reaching 1 at the
		 * specified speed.
		 *
		 * @param boostSpeed upper limit of low-speed boost effect, in km/h.
		 * @param boostGain boost gain at zero speed, typically greater than 1.
		 *     A gain of 1 will effectively disable the feature.
		 */
		void SetBoost(float boostSpeed, float boostGain);

		/**
		 * Returns the lumped constant that controls aerodynamic downforce.
		 * Technically speaking, this constant lumps together the reference
		 * area and the down-force coefficient, and is in world-units squared.
		 * The actual aerodynamic down-force is calculated as a function of
		 * current speed, and is proportional to this constant.
		 *
		 * @return the lumped constant that controls aerodynamic downforce.
		 */
		float GetDownforce() const;

		/**
		 * Sets the lumped constant that controls aerodynamic downforce.
		 * Technically speaking, this constant lumps together the reference
		 * area and the down-force coefficient, and is in world-units squared.
		 * The actual aerodynamic down-force is calculated as a function of
		 * current speed, and is proportional to this constant.
		 *
		 * @param downforce the lumped constant that controls aerodynamic downforce.
		 *     A value of 0 will effectively disable this feature.
		 */
		void SetDownforce(float downforce);

	protected:

		/**
		 * @see PhysicsCollisionObject::getCollisionObject
		 */
		btCollisionObject* GetCollisionObject() const;

	private:

		/**
		 * Creates a vehicle based on the specified rigid body parameters and some 'safe' defaults.
		 * 
		 * @param node The node to create a rigid body for; note that the node must have
		 *      a model attached to it prior to creating a rigid body for it.
		 * @param shape The rigid body shape construction information.
		 * @param parameters The rigid body construction parameters.
		 */
		PhysicsVehicle(Node* node, const PhysicsCollisionShape::Definition& shape, const PhysicsRigidBody::Parameters& parameters);

		/**
		 * Creates a vehicle based on the given rigid body and some 'safe' defaults.
		 * 
		 * @param node The node to create a rigid body for; note that the node must have
		 *      a model attached to it prior to creating a rigid body for it.
		 * @param rigidBody The rigid body.
		 */
		PhysicsVehicle(Node* node, PhysicsRigidBody* rigidBody);

		/**
		 * Private copy constructor to prevent copying.
		 */
		PhysicsVehicle(const PhysicsVehicle& src);

		/**
		 * Private copy assignment operator.
		 */
		PhysicsVehicle& operator=(const PhysicsVehicle&);

		/**
		 * Initializes this vehicle and advertises itself among its ancestor nodes.
		 */
		void Initialize();

		/**
		 * Destructor.
		 */
		~PhysicsVehicle();

		/**
		 * Returns adjusted steering value.
		 *
		 * @param v vehicle speed.
		 * @param rawSteering raw steering command.
		 */
		float GetSteering(float v, float rawSteering) const;

		/**
		 * Returns adjusted braking force value.
		 *
		 * @param v vehicle speed.
		 * @param rawBraking raw braking force command.
		 */
		float GetBraking(float v, float rawBraking) const;

		/**
		 * Returns adjusted driving force value.
		 *
		 * @param v vehicle speed.
		 * @param rawDriving raw driving force command.
		 * @param rawBraking raw braking force command.
		 */
		float GetDriving(float v, float rawDriving, float rawBraking) const;

		/**
		 * Applies effect of aerodynamic downforce.
		 */
		void ApplyDownforce();

		float steering_gain_;
		float braking_force_;
		float driving_force_;
		float steerdown_speed_;
		float steerdown_gain_;
		float brakedown_start_;
		float brakedown_full_;
		float drivedown_start_;
		float drivedown_full_;
		float boost_speed_;
		float boost_gain_;
		float downforce_;
		float speed_smoothed_;
		PhysicsRigidBody * rigid_body_;
		btRaycastVehicle::btVehicleTuning vehicle_tuning_;
		btVehicleRaycaster * vehicle_raycaster_;
		btRaycastVehicle * vehicle_;
		std::vector<PhysicsVehicleWheel*> wheels_;
	};

} // namespace scythe

#endif