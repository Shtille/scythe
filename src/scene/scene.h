#ifndef __SCYTHE_SCENE_H__
#define __SCYTHE_SCENE_H__

#include "resource.h"
#include "string_id.h"

#include <vector>

namespace scythe {

	/**
	 * Defines base scene class
	 */
	class Scene {
	public:
		Scene();
		virtual ~Scene();

		/**
		 * Attaches scene to be loaded with this one
		 */
		void Attach(Scene * scene);

		/**
		 * Detaches scene
		 */
		void Detach();
		Scene * attached_scene();

		virtual void Update() = 0;
		virtual void UpdatePhysics(float sec);
		virtual void Render() = 0;

		virtual void Load();
		virtual void Unload();

		void RequestLoad();
		void RequestUnload();

	protected:
		ResourceID AddResourceIdByName(StringID string_id);

		Scene * attached_scene_;
		std::vector<ResourceID> resources_;
	};

} // namespace scythe

#endif