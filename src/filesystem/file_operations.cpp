#include "file_operations.h"

#include <stdio.h>

namespace scythe {

	bool RemoveFile(const char* filename)
	{
		return ::remove(filename) == 0;
	}
	bool RenameFile(const char* old_name, const char* new_name)
	{
		return ::rename(old_name, new_name) == 0;
	}
	U64 ObtainFileSize(const char* filename)
	{
		U64 size(0ULL);
		FILE * file;
		file = fopen(filename, "rb");
		if (file)
		{
			fseek(file, 0, SEEK_END);
			size = static_cast<U64>(ftell(file));
			fclose(file);
		}
		return size;
	}

} // namespace scythe