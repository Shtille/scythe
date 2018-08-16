#ifndef __SCYTHE_PLATFORM_WINDOW_STRUCT_H__
#define __SCYTHE_PLATFORM_WINDOW_STRUCT_H__

#include "common/platform.h"

#if defined(SCYTHE_TARGET_MAC)

#if defined(__OBJC__)
 #import <Cocoa/Cocoa.h>
#else
 #include <ApplicationServices/ApplicationServices.h>
 typedef void* id;
#endif

#endif

struct PlatformWindow {
#if defined(SCYTHE_TARGET_WINDOWS)
	HWND hwnd;
	HDC dc;
	HGLRC rc;

	HINSTANCE instance;

	struct WindowState {
		RECT rect;
		LONG_PTR style;
		LONG_PTR ex_style;
	};
	WindowState old_state;
	WindowState current_state;

	HICON icon;
#elif defined(SCYTHE_TARGET_MAC)
	// App delegate
	CGEventSourceRef event_source;
	id app_delegate;
	id autorelease_pool;

	// Window delegate
	id object;
	id delegate;
	id view;

	// OpenGL specific
	id context;
	id pixel_format;
#elif defined(SCYTHE_TARGET_UNIX)
	// TODO:
#else
 #error PlatformWindow hasn't been implemented for this platform.
#endif
	bool need_quit;
};

#endif