#ifndef __SCYTHE_PLATFORM_INCLUDES_H__
#define __SCYTHE_PLATFORM_INCLUDES_H__

#include "platform.h"

#ifdef SCYTHE_TARGET_WINDOWS

	#ifdef _MSC_VER
	#include <SDKDDKVer.h>
	#endif

	#define WIN32_LEAN_AND_MEAN				// Exclude rarely-used stuff from Windows headers
	#ifndef NOMINMAX
	# define NOMINMAX 1						// We don't want these annoying defines
	#endif

	#include <Windows.h>
	#include <stddef.h>

#endif

#endif // __SCYTHE_PLATFORM_INCLUDES_H__