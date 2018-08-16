#include "context.h"
#include "common/platform.h"

#include <cstdio>

namespace scythe {

	Context::Context()
	{
		
	}
	Context::~Context()
	{
		
	}
	void Context::ErrorHandler(const char *message)
	{
#if defined(SCYTHE_TARGET_WINDOWS)
		OutputDebugStringA(message);
#else
		fprintf(stdout, "%s\n", message);
#endif
	}

} // namespace scythe