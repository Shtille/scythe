#ifndef __SCYTHE_DIRECTORY_H__
#define __SCYTHE_DIRECTORY_H__

namespace scythe {

	const char GetPathDelimeter();
	bool CreateDirectory(const char* path);
	bool RemoveDirectory(const char* path);

} // namespace scythe

#endif