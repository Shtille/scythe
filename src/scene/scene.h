#ifndef __SCYTHE_SCENE_H__
#define __SCYTHE_SCENE_H__

#include "resource.h"
#include "string_id.h"
#include "graphics/renderer.h"
#include "input/keys.h"
#include "input/mouse.h"

#include <vector>

namespace scythe {

	class Scene {
	public:
		Scene(Renderer * renderer);
		virtual ~Scene();

		void SetNextScene(Scene * scene);
		Scene * next();

		virtual void Update() = 0;
		virtual void UpdatePhysics(float sec);
		virtual void Render() = 0;

		virtual void Load();
		virtual void Unload();

		void RequestLoad();
		void RequestUnload();

		// TODO: remove input processing from here
		// Desktop-based user input message processing
		virtual void OnChar(unsigned short code);
		virtual void OnKeyDown(PublicKey key, int mods);
		virtual void OnMouseDown(MouseButton button, int modifiers, float x, float y);
		virtual void OnMouseUp(MouseButton button, int modifiers, float x, float y);
		virtual void OnMouseMove(float x, float y);

	protected:
		ResourceID AddResourceIdByName(StringID string_id);

		Renderer * renderer_;
		Scene * next_;
		std::vector<ResourceID> resources_;
	};

} // namespace scythe

#endif