#ifndef __SCYTHE_PHYSICS_VEHICLE_WHEEL_H__
#define __SCYTHE_PHYSICS_VEHICLE_WHEEL_H__

#include "physics_collision_object.h"
#include "physics_rigid_body.h"

namespace scythe {

	class PhysicsVehicle;

	/**
	 * Defines a class for vehicle wheel physics which represents the individual wheel
	 * itself as well as the tire and suspension.
	 *
	 * @see http://gameplay3d.github.io/GamePlay/docs/file-formats.html#wiki-Collision_Objects
	 */

	class PhysicsVehicleWheel : public PhysicsCollisionObject
	{
		friend class Node;
		friend class PhysicsVehicle;

	public:

		/**
		 * @see PhysicsCollisionObject#type
		 */
		PhysicsCollisionObject::Type type() const override;

		/**
		 * Operation not supported. Use host vehicle's SetEnabled() instead.
		 */
		void SetEnabled(bool enable);

		/**
		 * Apply this wheel's world transform to the specified node.
		 * Useful for updating the specified visual node with the current
		 * transform.
		 *
		 * @param node the node to be transformed; (typically a visual
		 * representation of this wheel).
		 */
		void Transform(Node* node) const;

		/**
		 * Returns true if this wheel is steerable, false otherwise.
		 *
		 * @return true if this wheel is steerable, false otherwise.
		 */
		bool IsSteerable() const;

		/**
		 * Sets whether this wheel is steerable.
		 *
		 * @param steerable true if this wheel is steerable, false otherwise.
		 */
		void SetSteerable(bool steerable);

		/**
		 * Gets direction of strut extension, in chassis space.
		 *
		 * @param wheelDirection address of where to store the result.
		 */
		void GetWheelDirection(Vector3* wheelDirection) const;

		/**
		 * Sets direction of strut extension, in chassis space.
		 *
		 * @param wheelDirection direction of strut extension.
		 */
		void SetWheelDirection(const Vector3& wheelDirection);

		/**
		 * Gets direction of axle (the spin axis), in chassis space.
		 *
		 * @param wheelAxle address of where to store the result.
		 */
		void GetWheelAxle(Vector3* wheelAxle) const;

		/**
		 * Sets direction of axle (the spin axis), in chassis space.
		 *
		 * @param wheelAxle direction of axle (the spin axis).
		 */
		void SetWheelAxle(const Vector3& wheelAxle);

		/**
		 * Gets offset from the default strut connection point.
		 * The default strut connection point is determined from the position
		 * of the wheel node relative to the chassis node, and uses the
		 * specified value for strut rest length to locate the connection
		 * point above it (i.e., in the specified direction of strut
		 * compression).
		 * Any non-zero strut connection offset acts as a delta from the
		 * computed default.
		 *
		 * @param strutConnectionOffset address of where to store the result.
		 */
		void GetStrutConnectionOffset(Vector3* strutConnectionOffset) const;

		/**
		 * Sets offset from the default strut connection point.
		 * The default strut connection point is determined from the position
		 * of the wheel node relative to the chassis node, and uses the
		 * specified value for strut rest length to locate the connection
		 * point above it (i.e., in the specified direction of strut
		 * compression).
		 * Any non-zero strutConnectionOffset acts as a delta from the
		 * computed default.
		 *
		 * @param strutConnectionOffset offset from the default strut connection
		 *     point.
		 */
		void SetStrutConnectionOffset(const Vector3& strutConnectionOffset);

		/**
		 * Gets the strut rest length.
		 *
		 * @return the strut rest length.
		 */
		float GetStrutRestLength() const;

		/**
		 * Sets the strut rest length.
		 *
		 * @param strutRestLength the strut rest length.
		 */
		void SetStrutRestLength(float strutRestLength);

		/**
		 * Gets the maximum strut travel.
		 *
		 * @return the maximum strut travel.
		 */
		float GetStrutTravelMax() const;

		/**
		 * Sets the maximum strut travel.
		 *
		 * @param strutTravelMax the maximum strut travel.
		 */
		void SetStrutTravelMax(float strutTravelMax);

		/**
		 * Gets the strut stiffness, normalized to chassis mass.
		 *
		 * @return the strut stiffness, normalized to chassis mass.
		 */
		float GetStrutStiffness() const;

		/**
		 * Sets the strut stiffness, normalized to chassis mass.
		 *
		 * @param strutStiffness the strut stiffness, normalized to chassis mass.
		 */
		void SetStrutStiffness(float strutStiffness);

		/**
		 * Gets strut damping under compression, normalized to chassis mass.
		 *
		 * @return strut damping under compression, normalized to chassis mass.
		 */
		float GetStrutDampingCompression() const;

		/**
		 * Sets strut damping under compression, normalized to chassis mass.
		 *
		 * @param strutDampingCompression strut damping under compression, normalized to chassis mass.
		 */
		void SetStrutDampingCompression(float strutDampingCompression);

		/**
		 * Gets strut damping under relaxation, normalized to chassis mass.
		 *
		 * @return strut damping under relaxation, normalized to chassis mass.
		 */
		float GetStrutDampingRelaxation() const;

		/**
		 * Sets strut damping under relaxation, normalized to chassis mass.
		 *
		 * @param strutDampingRelaxation strut damping under relaxation, normalized to chassis mass.
		 */
		void SetStrutDampingRelaxation(float strutDampingRelaxation);

		/**
		 * Gets the maximum strut force.
		 *
		 * @return the maximum strut force.
		 */
		float GetStrutForceMax() const;

		/**
		 * Sets the maximum strut force.
		 *
		 * @param strutForceMax the maximum strut force.
		 */
		void SetStrutForceMax(float strutForceMax);

		/**
		 * Gets the breakout friction.
		 *
		 * @return the breakout friction.
		 */
		float GetFrictionBreakout() const;

		/**
		 * Sets the breakout friction.
		 *
		 * @param frictionBreakout the breakout friction.
		 */
		void SetFrictionBreakout(float frictionBreakout);

		/**
		 * Gets the wheel radius.
		 *
		 * @return the wheel radius.
		 */
		float GetWheelRadius() const;

		/**
		 * Sets the wheel radius.
		 *
		 * @param wheelRadius the wheel radius.
		 */
		void SetWheelRadius(float wheelRadius);

		/**
		 * Gets roll influence which determines how side friction affects chassis roll.
		 *
		 * @return roll influence, normalized factor.
		 */
		float GetRollInfluence() const;

		/**
		 * Sets roll influence which determines how side friction affects chassis roll.
		 *
		 * @param rollInfluence roll influence, normalized factor.
		 */
		void SetRollInfluence(float rollInfluence);

	protected:

		/**
		 * @see PhysicsCollisionObject::getCollisionObject
		 */
		btCollisionObject* GetCollisionObject() const override;

	private:

		/**
		 * Creates a vehicle wheel based on the specified rigid body parameters and some 'safe' defaults.
		 * Also, traverse up the scene graph until we find the first common ancestor with another node
		 * of collision type VEHICLE and add ourself as a wheel onto that vehicle. This assumes that the
		 * VEHICLE comes before the VEHICLE_WHEEL in the ".scene" (properties) file.
		 * 
		 * @param node The node to create a rigid body for; note that the node must have
		 *      a model attached to it prior to creating a rigid body for it.
		 * @param shape The rigid body shape construction information.
		 * @param parameters The rigid body construction parameters.
		 */
		PhysicsVehicleWheel(Node* node, const PhysicsCollisionShape::Definition& shape, const PhysicsRigidBody::Parameters& parameters);

		/**
		 * Creates a vehicle wheel based on some 'safe' defaults.
		 * Also, traverse up the scene graph until we find the first common ancestor with another node
		 * of collision type VEHICLE and add ourself as a wheel onto that vehicle. This assumes that the
		 * VEHICLE comes before the VEHICLE_WHEEL in the ".scene" (properties) file.
		 * 
		 * @param node The node to create a vehicle wheel for.
		 */
		PhysicsVehicleWheel(Node* node);

		/**
		 * Private copy constructor to prevent copying.
		 */
		PhysicsVehicleWheel(const PhysicsVehicleWheel& src);

		/**
		 * Private copy assignment operator.
		 */
		PhysicsVehicleWheel& operator=(const PhysicsVehicleWheel&);

		/**
		 * Destructor.
		 */
		~PhysicsVehicleWheel();

		/**
		 * Traverse up the visual scene graph. Upon finding the first ancestor node with an
		 * advertised descendant of collision type VEHICLE, add this wheel onto the vehicle.
		 */
		// Note: Currently this method is silent on failure to find a host.
		void FindAncestorAndBind();

		/**
		 * Depth-first search for the first vehicle starting from the specified node.
		 *
		 * @param node the starting node for the recursive search.
		 */
		PhysicsVehicle* FindVehicle(Node* node);

		/**
		 * Sets the host vehicle for this wheel.
		 *
		 * @param host the host vehicle.
		 * @param indexInHost the index of this wheel within the host vehicle.
		 */
		void SetHost(PhysicsVehicle* host, unsigned int indexInHost);

		/**
		 * Adds this wheel to the specified Bullet vehicle.
		 *
		 * @param vehicle Bullet vehicle to add self to.
		 */
		void AddToVehicle(btRaycastVehicle* vehicle);

		/**
		 * Update state of this wheel, per frame.
		 *
		 * @param elapsedTime The elapsed game time.
		 */
		void Update(float elapsedTime);

		/**
		 * Computes the default strut connection point for
		 * this wheel.
		 * The default strut connection point is determined from the position
		 * of the wheel node relative to the chassis node, and uses the
		 * specified value for maximum strut travel to locate the connection
		 * point above it (i.e., in the specified direction of strut
		 * compression).
		 *
		 * @param result where to store the result.
		 */
		void GetConnectionDefault(Vector3* result) const;

		/**
		 * Get wheel position at bind time relative to chassis.
		 *
		 * @param result where to store the result.
		 */
		void GetWheelPos(Vector3* result) const;

		PhysicsVehicle* host_;
		unsigned int index_in_host_;
		Vector3 initial_offset_;
		Vector3 position_delta_;
		Quaternion orientation_;
	};

} // namespace scythe

#endif