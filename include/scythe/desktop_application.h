#ifndef __SCYTHE_DESKTOP_APPLICATION_H__
#define __SCYTHE_DESKTOP_APPLICATION_H__

#include "application.h"
#include "keyboard.h"
#include "mouse.h"
#include "keyboard_controller.h"
#include "mouse_controller.h"
#include "window_controller.h"

namespace scythe {

	/**
	 * @brief      This class describes a desktop application.
	 */
	class DesktopApplication
	: public Application
	{
	public:
		DesktopApplication();

		static DesktopApplication* GetInstance();

		KeyboardState& GetKeyboardState();
		MouseState& GetMouseState();

		KeyboardController* GetKeyboardController();
		MouseController* GetMouseController();
		WindowController* GetWindowController();

		// Window actions
		void ToggleFullscreen();

		// Window properties
		const int GetWidth() const;
		const int GetHeight() const;
		const float GetAspectRatio() const;
		const bool IsActive() const;
		const bool IsVisible() const;
		const bool IsFullscreen() const;

		// Tuneable application styles
		virtual const int GetInitialWidth() const;
		virtual const int GetInitialHeight() const;
		virtual const bool IsInitialFullscreen() const;
		virtual const bool IsDecorated() const;
		virtual const bool IsResizable() const;

	private:
		bool CreateSurface() override final;
		void DestroySurface() override final;

	protected:
		KeyboardState			keyboard_state_;		//!< keyboard state
		MouseState				mouse_state_;			//!< mouse state
		KeyboardController* 	keyboard_controller_;	//!< keyboard controller
		MouseController* 		mouse_controller_;		//!< mouse controller
		WindowController*		window_controller_;		//!< window controller
	};

} // namespace scythe

#endif