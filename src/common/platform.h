#ifndef __SCYTHE_PLATFORM_H__
#define __SCYTHE_PLATFORM_H__

#if defined(_WIN32) // Windows

	#define SCYTHE_TARGET_WINDOWS

	#ifdef _MSC_VER
	#include <SDKDDKVer.h>
	#endif

	#define WIN32_LEAN_AND_MEAN				// Exclude rarely-used stuff from Windows headers
	#ifndef NOMINMAX
	# define NOMINMAX 1						// We don't want these annoying defines
	#endif

	#include <Windows.h>

	// Avoid conflictable Windows defines:
	#ifdef CreateDirectory
	#undef CreateDirectory
	#endif // CreateDirectory

	#ifdef RemoveDirectory
	#undef RemoveDirectory
	#endif // RemoveDirectory

	#include <stddef.h>

#elif defined(__unix__) || defined(__unix) // Linux

	#define SCYTHE_TARGET_UNIX

#elif defined(__APPLE__) && defined(__MACH__) // Mac OS X

	#include <TargetConditionals.h>
	#if TARGET_IPHONE_SIMULATOR == 1
		/* iOS in Xcode simulator */
		#define SCYTHE_TARGET_IOS
	#elif TARGET_OS_IPHONE == 1
		/* iOS on iPhone, iPad, etc. */
		#define SCYTHE_TARGET_IOS
	#elif TARGET_OS_MAC == 1
		/* OSX */
		#define SCYTHE_TARGET_MAC
	#else
		#error "Unsupported Apple device"
	#endif
#else

	#error "Unknown target platform"

#endif

#if defined(SCYTHE_TARGET_WINDOWS) || defined(SCYTHE_TARGET_UNIX) || defined(SCYTHE_TARGET_MAC)
	#define SCYTHE_TARGET_DESKTOP
#else
	#define SCYTHE_TARGET_MOBILE
#endif

#include <assert.h>

#endif