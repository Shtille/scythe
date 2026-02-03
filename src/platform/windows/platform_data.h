#ifndef __SCYTHE_PLATFORM_DATA_H__
#define __SCYTHE_PLATFORM_DATA_H__

#include "../base_window.h"
#include <scythe/platform_includes.h>

namespace scythe::platform {

#ifdef SCYTHE_USE_OPENGL
	/**
	 * @brief      Rendering context implementation for Windows platform
	 */
	struct RenderingContext
	{
		HDC dc;
		HGLRC handle;
	};
#endif

	/**
	 * @brief      Window implementation for Windows platform
	 */
	struct Window
	{
		// Base window struct to avoid inheritance
		BaseWindow base;

#ifdef SCYTHE_USE_OPENGL
		RenderingContext context;
#endif
		HWND handle;

		struct State
		{
			RECT rect;
			POINT pos;
			DWORD style;
			DWORD ex_style;
		};
		State windowed_state;
		State fullscreen_state;

		POINT old_mouse_position;
	};

	/**
	 * @brief      Describes data specific for Windows platform.
	 */
	struct Data
	{
		HINSTANCE instance;

		HICON icon;

		HWND helper_window_handle;
		Window* main_window;
		bool helper_window_class_registered;
		bool main_window_class_registered;
	};

} // namespace scythe::platform

#endif