#ifndef __SCYTHE_PLATFORM_WINDOW_H__
#define __SCYTHE_PLATFORM_WINDOW_H__

#include <scythe/window.h>

#if defined(__OBJC__)
	#import <Cocoa/Cocoa.h>
#else
	#include <ApplicationServices/ApplicationServices.h>
	typedef void* id;
#endif

struct PlatformWindow
{
	// Base window struct to avoid inheritance
	Window base;

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
};

#endif