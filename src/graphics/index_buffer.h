#ifndef __SCYTHE_INDEX_BUFFER_H__
#define __SCYTHE_INDEX_BUFFER_H__

#include "video_memory_buffer.h"

namespace scythe {

	class IndexBuffer final : public VideoMemoryBuffer {
		friend class Renderer;
		friend class OpenGlRenderer;

	protected:
		IndexBuffer(Context * context);
		~IndexBuffer();
		IndexBuffer(const IndexBuffer&) = delete;
		void operator = (const IndexBuffer&) = delete;
		
		U32 GetSize();
		
		void Bind();
		void Unbind();
		
		void SetData(U32 size, const void *data, BufferUsage usage);
		void SubData(U32 size, const void *data);
		
		void* Lock(DataAccessType access);
		void Unlock();
		
	private:
		U32 id_;
		U32 index_count_;
		U32 index_size_;
	};

} // namespace scythe

#endif