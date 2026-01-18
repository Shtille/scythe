#ifndef __SCYTHE_PLATFORM_INCLUDES_H__
#define __SCYTHE_PLATFORM_INCLUDES_H__

#include "platform.h"

#ifdef SCYTHE_TARGET_WINDOWS

	// We don't need all the fancy stuff
	#ifndef NOMINMAX
	# define NOMINMAX
	#endif

	#ifndef VC_EXTRALEAN
	# define VC_EXTRALEAN
	#endif

	#ifndef WIN32_LEAN_AND_MEAN
	# define WIN32_LEAN_AND_MEAN
	#endif

	#ifdef _MSC_VER
	# include <SDKDDKVer.h>
	#endif

	#include <Windows.h>
	#include <stddef.h>

#endif

#endif // __SCYTHE_PLATFORM_INCLUDES_H__