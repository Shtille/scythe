#include "pool_allocator.h"

#include "common/sc_assert.h"

namespace scythe {

	PoolAllocator::PoolAllocator(const size_t num_chunks)
	: num_chunks_(num_chunks)
	, chunk_size_(0)
#ifdef SCYTHE_MEMORY_DEBUG
	, total_size_(0)
	, used_(0)
#endif
	{
	}
	PoolAllocator::PoolAllocator(const PoolAllocator& other)
	: num_chunks_(other.num_chunks_)
	, chunk_size_(0)
#ifdef SCYTHE_MEMORY_DEBUG
	, total_size_(0)
	, used_(0)
#endif
	{
	}
	PoolAllocator::PoolAllocator(PoolAllocator&& other)
	: num_chunks_(other.num_chunks_)
	, chunk_size_(other.chunk_size_)
#ifdef SCYTHE_MEMORY_DEBUG
	, total_size_(other.total_size_)
	, used_(other.used_)
#endif
	, free_list_(std::move(other.free_list_))
	, buffers_(std::move(other.buffers_))
	{
		other.chunk_size_ = 0;
#ifdef SCYTHE_MEMORY_DEBUG
		other.total_size_ = 0;
		other.used_ = 0;
#endif
	}
	PoolAllocator::~PoolAllocator()
	{
		for (auto buffer : buffers_)
			delete[] buffer;
	}
	void * PoolAllocator::Allocate(const size_t allocation_size)
	{
		SC_ASSERT((buffers_.empty() || sizeof(Node) + allocation_size == chunk_size_) && "Allocated size should be constant");
		Node * free_node = free_list_.pop();
		if (free_node == NULL)
		{
			if (buffers_.empty())
			{
				// First time buffer allocation
				chunk_size_ = sizeof(Node) + allocation_size;
			}
#ifdef SCYTHE_MEMORY_DEBUG
			total_size_ += num_chunks_ * chunk_size_;
#endif
			// The pool allocator is full
			// Add a new pool
			unsigned char* buffer = AllocateBuffer();
			// Create a linked-list with all free positions
			for (size_t i = 0; i < num_chunks_; ++i)
			{
				unsigned char* node_ptr = buffer + i * chunk_size_;
				free_list_.push(reinterpret_cast<Node*>(node_ptr));
			}
			free_node = free_list_.pop();
		}
#ifdef SCYTHE_MEMORY_DEBUG
		used_ += chunk_size_;
#endif
		unsigned char* address = reinterpret_cast<unsigned char*>(free_node) + sizeof(Node);
		return reinterpret_cast<void*>(address);
	}
	void PoolAllocator::Free(void * ptr)
	{
#ifdef SCYTHE_MEMORY_DEBUG
		used_ -= chunk_size_;
#endif
		unsigned char* node_ptr = reinterpret_cast<unsigned char*>(ptr) - sizeof(Node);
		free_list_.push(reinterpret_cast<Node*>(node_ptr));
	}
	unsigned char* PoolAllocator::AllocateBuffer()
	{
		size_t size_to_allocate = num_chunks_ * chunk_size_;
		unsigned char* buffer = new unsigned char[size_to_allocate];
		buffers_.push_back(buffer);
		return buffer;
	}
	size_t PoolAllocator::num_chunks() const
	{
		return num_chunks_;
	}

} // namespace scythe