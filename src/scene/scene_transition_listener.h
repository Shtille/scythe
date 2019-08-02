#ifndef __SCYTHE_SCENE_TRANSITION_LISTENER_H__
#define __SCYTHE_SCENE_TRANSITION_LISTENER_H__

namespace scythe {

	/**
	 * Defines scene transition listener interface
	 */
	class SceneTransitionListener {
	public:
		virtual ~SceneTransitionListener() = default;

		/**
		 * Notification of how many steps are expected
		 * @param count Number of steps.
		 */
		virtual void OnObtainCount(int count) = 0;

		/**
		 * Notification of one step completion
		 */
		virtual void OnStep() = 0;

		/**
		 * Notification of transition finish
		 */
		virtual void OnFinish() = 0;
	};

} // namespace scythe

#endif