#ifndef __SCYTHE_PLATFORM_DATA_H__
#define __SCYTHE_PLATFORM_DATA_H__

#include <scythe/platform_includes.h>

#if defined(__OBJC__)
	#import <Cocoa/Cocoa.h>
#else
	#include <ApplicationServices/ApplicationServices.h>
	typedef void* id;
#endif

// Forward
#ifdef __cplusplus
namespace scythe {
	struct BaseWindow;
}
#endif

namespace scythe::platform {

#ifdef SCYTHE_USE_OPENGL
	/**
	 * @brief      Rendering context implementation for Mac OS X platform
	 */
	struct RenderingContext
	{
		id object;
		id pixel_format;
	};
#endif

	/**
	 * @brief      Window implementation for Mac OS X platform
	 */
	struct Window
	{
#ifdef __cplusplus
		BaseWindow* base;
#else
		void* base;
#endif

		// Window delegate
		id object;
		id delegate;
		id view;

#ifdef SCYTHE_USE_OPENGL
		RenderingContext context;
#endif
	};

	/**
	 * @brief      Describes data specific for Mac OS X platform.
	 */
	struct Data
	{
		// App delegate
		CGEventSourceRef event_source;
		id app_delegate;
		id autorelease_pool;

#ifdef __cplusplus
		Window* main_window;
#else
		void* main_window;
#endif
	};

} // namespace scythe::platform

#endif