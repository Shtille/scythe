#ifndef __SCYTHE_PHYSICS_CONTROLLER_H__
#define __SCYTHE_PHYSICS_CONTROLLER_H__

namespace scythe {

	/**
	 * @brief      Controls how physics objects are going to be updated.
	 */
	class PhysicsController
	{
	public:
		virtual ~PhysicsController() = default;

		virtual void UpdatePhysics(float sec) = 0;
	};

} // namespace scythe

#endif