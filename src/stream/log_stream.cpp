#include "log_stream.h"

namespace scythe {

	ScopeLogStream::ScopeLogStream(const char *filename)
	{
		bool opened = Open(filename, StreamAccess::kAppend);
		SC_ASSERT(opened);
	}

} // namespace scythe