#ifndef __SCYTHE_KEYBOARD_CONTROLLER_H__
#define __SCYTHE_KEYBOARD_CONTROLLER_H__

#include "keyboard.h"

namespace scythe {
	
	/**
	 * Defines a keyboard controller
	 */
	class KeyboardController
	{
	public:
		virtual ~KeyboardController();

		virtual void OnChar(KeyCode code) {}
		virtual void OnKeyDown(KeyboardKey key, KeyModifiers modifiers) {}
		virtual void OnKeyUp(KeyboardKey key, KeyModifiers modifiers) {}
	};
	
} // namespace scythe

#endif // __SCYTHE_KEYBOARD_CONTROLLER_H__