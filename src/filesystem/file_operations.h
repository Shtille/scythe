#ifndef __SCYTHE_FILE_OPERATIONS_H__
#define __SCYTHE_FILE_OPERATIONS_H__

#include "common/types.h"

namespace scythe {

	bool RemoveFile(const char* filename);
	bool RenameFile(const char* old_name, const char* new_name);

	U64 ObtainFileSize(const char* filename);

} // namespace scythe

#endif