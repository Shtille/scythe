#ifndef __SCYTHE_PLATFORM_DATA_H__
#define __SCYTHE_PLATFORM_DATA_H__

#include <scythe/platform_includes.h>

#if defined(__OBJC__)
	#import <Cocoa/Cocoa.h>
#else
	#include <ApplicationServices/ApplicationServices.h>
	typedef void* id;
#endif

#ifdef SCYTHE_USE_OPENGL
	/**
	 * @brief      Rendering context implementation for Mac OS X platform
	 */
	struct PlatformRenderingContext
	{
		id context;
		id pixel_format;
	};
#endif

	/**
	 * @brief      Window implementation for Mac OS X platform
	 */
	struct PlatformWindow
	{
		// Base window struct to avoid inheritance
		BaseWindow base;

		// Window delegate
		id object;
		id delegate;
		id view;

#ifdef SCYTHE_USE_OPENGL
		PlatformRenderingContext context;
#endif
	};

	/**
	 * @brief      Describes data specific for Mac OS X platform.
	 */
	struct PlatformData
	{
		// App delegate
		CGEventSourceRef event_source;
		id app_delegate;
		id autorelease_pool;

		PlatformWindow* main_window;
	};

#endif