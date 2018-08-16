#ifndef __SCYTHE_ASSERT_H__
#define __SCYTHE_ASSERT_H__

#if defined(DEBUG) || defined(_DEBUG)
	#include <assert.h>
	#define SC_ASSERT(x) assert(x)
#else
	#define SC_ASSERT(x) (void)(x)
#endif

#endif