#ifndef __SCYTHE_DESKTOP_INPUT_LISTENER_H__
#define __SCYTHE_DESKTOP_INPUT_LISTENER_H__

#include "input_listener.h"
#include "keys.h"
#include "mouse.h"

namespace scythe {
	
	/**
	 * Defines a desktop input listener interface
	 */
	class DesktopInputListener : public InputListener {
	public:
		virtual void OnChar(unsigned short code) = 0;
		virtual void OnKeyDown(PublicKey key, int modifiers) = 0;
		virtual void OnKeyUp(PublicKey key, int modifiers) = 0;
		virtual void OnMouseDown(MouseButton button, int modifiers) = 0;
		virtual void OnMouseUp(MouseButton button, int modifiers) = 0;
		virtual void OnMouseMove() = 0;
		virtual void OnScroll(float delta_x, float delta_y) = 0;
	};
	
} // namespace scythe

#endif