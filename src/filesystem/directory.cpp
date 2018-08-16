#include "directory.h"

#include "common/platform.h"

#ifndef SCYTHE_TARGET_WINDOWS
#include <sys/stat.h>
#include <unistd.h>
#endif // !SCYTHE_TARGET_WINDOWS

namespace scythe {

	const char GetPathDelimeter()
	{
#ifdef SCYTHE_TARGET_WINDOWS
		return '\\';
#else
		return '/';
#endif // SCYTHE_TARGET_WINDOWS
	}
	bool CreateDirectory(const char* path)
	{
#ifdef SCYTHE_TARGET_WINDOWS
		return CreateDirectoryA(path, NULL) != 0;
#else
		return mkdir(path, 0755) == 0;
#endif // SCYTHE_TARGET_WINDOWS
	}
	bool RemoveDirectory(const char* path)
	{
#ifdef SCYTHE_TARGET_WINDOWS
		return RemoveDirectoryA(path) != 0;
#else
		return rmdir(path) == 0;
#endif // SCYTHE_TARGET_WINDOWS
	}

} // namespace scythe