#ifndef __SCYTHE_SCENE_TRANSITION_LISTENER_H__
#define __SCYTHE_SCENE_TRANSITION_LISTENER_H__

namespace scythe {

	class SceneTransitionListenerInterface {
	public:
		virtual ~SceneTransitionListenerInterface() = default;

		virtual void OnObtainCount(int count) = 0;
		virtual void OnStep() = 0;
		virtual void OnFinish() = 0;
	};

} // namespace scythe

#endif