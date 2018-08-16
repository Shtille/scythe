#ifndef __SCYTHE_VIDEO_MEMORY_BUFFER_H__
#define __SCYTHE_VIDEO_MEMORY_BUFFER_H__

#include "common/types.h"
#include "context.h"

namespace scythe {

	class VideoMemoryBuffer {
	public:
		VideoMemoryBuffer(Context * context);
		virtual ~VideoMemoryBuffer();
		
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		
		virtual void SetData(U32 size, const void *data, BufferUsage usage) = 0;
		virtual void SubData(U32 size, const void *data) = 0;
		
		virtual void* Lock(DataAccessType access) = 0;
		virtual void  Unlock() = 0;
		
	protected:
		Context * context_;
	};

} // namespace scythe

#endif