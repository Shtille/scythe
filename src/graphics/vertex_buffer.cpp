#include "vertex_buffer.h"

namespace scythe {

	VertexBuffer::VertexBuffer(Context * context)
	: VideoMemoryBuffer(context)
	{
		context_->GenVertexBuffer(id_);
	}
	VertexBuffer::~VertexBuffer()
	{
		context_->DeleteVertexBuffer(id_);
	}
	U32 VertexBuffer::GetSize()
	{
		return size_;
	}
	void VertexBuffer::Bind()
	{
		context_->BindVertexBuffer(id_);
	}
	void VertexBuffer::Unbind()
	{
		context_->BindVertexBuffer(0);
	}
	void VertexBuffer::SetData(U32 size, const void *data, BufferUsage usage)
	{
		context_->VertexBufferData(size, data, usage);
	}
	void VertexBuffer::SubData(U32 size, const void *data)
	{
		context_->VertexBufferSubData(size, data);
	}
	void* VertexBuffer::Lock(DataAccessType access)
	{
		return context_->MapVertexBufferData(access);
	}
	void VertexBuffer::Unlock()
	{
		context_->UnmapVertexBufferData();
	}

} // namespace scythe