#ifndef __SCYTHE_SCENE_TRANSITION_MANAGER_H__
#define __SCYTHE_SCENE_TRANSITION_MANAGER_H__

#include "scene_transition_listener.h"

namespace scythe {

	// Forward declarations
	class Scene;

	/**
	 * Defines scene transition type
	 */
	enum class SceneTransition {
		kNone,
		kImmediate,
		kDeferred,
		kAsyncronous
	};

	/**
	 * Defines scene manager
	 */
	class SceneManager {
	public:
		SceneManager();
		virtual ~SceneManager();

		void Update();
		void UpdatePhysics(float sec);
		void Render();

		void RequestImmediateTransition(Scene * scene);
		void RequestDeferredTransition(Scene * scene, Scene * loading_scene, SceneTransitionListener * listener);

	private:
		void UpdateTransition();
		bool ImmediateTransition();
		bool DeferredTransition();

		Scene * current_scene_;
		Scene * next_scene_;
		Scene * loading_scene_;
		SceneTransitionListener * listener_;
		int transition_phase_;
		int render_count_;				//!< number of render calls after transition
		SceneTransition transition_type_;
		bool transition_finalization_;	//!< whether we need to do some render calls after transition
	};

} // namespace scythe

#endif