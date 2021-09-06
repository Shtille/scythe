#ifndef __SCYTHE_POOL_ALLOCATOR_H__
#define __SCYTHE_POOL_ALLOCATOR_H__

#include "allocator.h"
#include "stack_linked_list.h"

#include <vector>

namespace scythe {

	/**
	 * Pool allocator.
	 * Allocates memory blocks with constant size.
	 */
	class PoolAllocator final : public Allocator {

		typedef StackLinkedList::Node Node;

	public:

		/**
		 * Constructor
		 *
		 * @param[in] num_chunks Number of chunks per single buffer
		 */
		PoolAllocator(const size_t num_chunks);

		/**
		 * Copy constructor
		 *
		 * @param[in] other Other allocator.
		 */
		PoolAllocator(const PoolAllocator& other);

		/**
		 * Move constructor
		 *
		 * @param[in] other Other allocator.
		 */
		PoolAllocator(PoolAllocator&& other);

		/**
		 * Destructor
		 */
		~PoolAllocator();

		/**
		 * Allocates block of memory
		 * Note that allocation size should be constant
		 *
		 * @param[in] allocation_size Size of memory block
		 */
		void * Allocate(const size_t allocation_size) final;

		/**
		 * Releases block of memory that was allocated previously
		 *
		 * @param[in] ptr Pointer to block of memory
		 */
		void Free(void * ptr) final;

		/**
		 * Returns number of chunks
		 */
		size_t num_chunks() const;

	private:

		/**
		 * Disallow default constructor
		 */
		PoolAllocator() = delete;

		unsigned char* AllocateBuffer();
		
		size_t num_chunks_;
		size_t chunk_size_;
#ifdef SCYTHE_MEMORY_DEBUG
		size_t total_size_;
		size_t used_;
#endif
		StackLinkedList free_list_;
		std::vector<unsigned char*> buffers_;
	};

} // namespace scythe

#endif