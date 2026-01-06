#ifndef __SCYTHE_PLATFORM_WINDOW_H__
#define __SCYTHE_PLATFORM_WINDOW_H__

#include <scythe/window.h>
#include <scythe/platform_includes.h>

namespace scythe {

	/**
	 * @brief      Window implementation for Windows platform
	 */
	struct PlatformWindow
	{
		// Base window struct to avoid inheritance
		Window base;

		HWND hwnd;
		HDC dc;
		HGLRC rc;

		HINSTANCE instance;

		struct WindowState
		{
			RECT rect;
			DWORD style;
			DWORD ex_style;
		};
		WindowState old_state;
		WindowState current_state;

		HICON icon;

		POINT old_mouse_position;
	};

} // namespace scythe

#endif