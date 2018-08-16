#ifndef __SCYTHE_SCENE_MANAGER_H__
#define __SCYTHE_SCENE_MANAGER_H__

#include "scene_transition_listener.h"
#include "event/event_listener.h"
#include "input/keys.h"
#include "input/mouse.h"

namespace scythe {

	class Scene;

	enum class Transition {
		kNone,
		kImmediate,
		kDeferred,
		kAsyncronous
	};

	class SceneManager : public EventListenerInterface {
	public:
		SceneManager();
		virtual ~SceneManager();

		void Update();
		void UpdatePhysics(float sec);
		void Render();

		// Desktop-based user input message processing
		void OnChar(unsigned short code);
		void OnKeyDown(PublicKey key, int mods);
		void OnMouseDown(MouseButton button, int modifiers, float x, float y);
		void OnMouseUp(MouseButton button, int modifiers, float x, float y);
		void OnMouseMove(float x, float y);

	protected:
		void RequestImmediateTransition(Scene * scene);
		void RequestDeferredTransition(Scene * scene, Scene * loading_scene, SceneTransitionListenerInterface * listener);

	private:
		void UpdateTransition();
		bool ImmediateTransition();
		bool DeferredTransition();

		Scene * current_scene_;
		Scene * next_scene_;
		Scene * loading_scene_;
		SceneTransitionListenerInterface * listener_;
		int transition_phase_;
		int render_count_;								//!< number of render calls after transition
		Transition transition_type_;
		bool transition_finalization_;					//!< whether we need to do some render calls after transition
	};

} // namespace scythe

#endif