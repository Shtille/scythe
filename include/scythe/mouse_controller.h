#ifndef __SCYTHE_MOUSE_CONTROLLER_H__
#define __SCYTHE_MOUSE_CONTROLLER_H__

#include "mouse.h"

namespace scythe {
	
	/**
	 * Defines a mouse controller
	 */
	class MouseController
	{
	public:
		virtual ~MouseController() = default;

		virtual void OnMouseDown(MouseButton button) {}
		virtual void OnMouseUp(MouseButton button) {}
		virtual void OnMouseMove() {}
		virtual void OnScroll(float delta_x, float delta_y) {}
	};
	
} // namespace scythe

#endif // __SCYTHE_MOUSE_CONTROLLER_H__