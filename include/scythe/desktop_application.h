#ifndef __SCYTHE_DESKTOP_APPLICATION_H__
#define __SCYTHE_DESKTOP_APPLICATION_H__

#include "application.h"
#include "keyboard.h"
#include "mouse.h"
#include "keyboard_controller.h"
#include "mouse_controller.h"
#include "window.h"

namespace scythe {

	// Forward declarations
	class DesktopApplication;
	struct PlatformWindow;
	namespace platform {
		bool Initialize();
		void Deinitialize();
		PlatformWindow* GetPlatformWindow(DesktopApplication*);
	}

	/**
	 * @brief      This class describes a desktop application.
	 */
	class DesktopApplication
	: public Application
	{
		friend bool platform::Initialize();
		friend void platform::Deinitialize();
		friend PlatformWindow* platform::GetPlatformWindow(DesktopApplication*);

	public:
		DesktopApplication();

		static DesktopApplication* GetInstance();

		KeyboardState& GetKeyboardState();
		MouseState& GetMouseState();

		KeyboardController* GetKeyboardController();
		MouseController* GetMouseController();

		// Window properties
		const Window* GetWindow() const;
		const int GetWidth() const;
		const int GetHeight() const;
		const float GetAspectRatio() const;
		const bool IsVisible() const;
		const bool IsFullscreen() const;

		// Tuneable application styles
		virtual const int GetInitialWidth() const;
		virtual const int GetInitialHeight() const;
		virtual const bool GetInitialFullscreen() const;
		virtual const bool IsDecorated() const;
		virtual const bool IsResizable() const;

		virtual void OnSize(int width, int height);

	private:
		bool CreateSurface() override final;
		void DestroySurface() override final;

	protected:
		KeyboardState			keyboard_state_;		//!< keyboard state
		MouseState				mouse_state_;			//!< mouse state
		KeyboardController* 	keyboard_controller_;	//!< keyboard controller
		MouseController* 		mouse_controller_;		//!< mouse controller

	private:
		void* 					platform_window_;		//!< platform window
	};

} // namespace scythe

#endif