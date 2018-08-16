#ifndef __SCYTHE_VERTEX_BUFFER_H__
#define __SCYTHE_VERTEX_BUFFER_H__

#include "video_memory_buffer.h"

namespace scythe {

	class VertexBuffer final : public VideoMemoryBuffer {
		friend class Renderer;
		friend class OpenGlRenderer;
		
		friend class DynamicText;
		
	protected:
		VertexBuffer(Context * context);
		~VertexBuffer();
		VertexBuffer(const VertexBuffer&) = delete;
		void operator = (const VertexBuffer&) = delete;
		
		U32 GetSize();
		
		void Bind();
		void Unbind();
		
		void SetData(U32 size, const void *data, BufferUsage usage);
		void SubData(U32 size, const void *data);
		
		void* Lock(DataAccessType access);
		void Unlock();
		
	private:
		U32 id_;
		U32 size_;
	};

} // namespace scythe

#endif