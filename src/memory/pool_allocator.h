#ifndef __SCYTHE_POOL_ALLOCATOR_H__
#define __SCYTHE_POOL_ALLOCATOR_H__

#include "stack_linked_list.h"

#include <vector>
#include <cstddef>

namespace scythe {

	/**
	 * Pool allocator.
	 * Allocates memory blocks with constant size.
	 */
	class PoolAllocator {

		typedef StackLinkedList::Node Node;

	public:

		/**
		 * Constructor
		 *
		 * @param[in] num_chunks Number of chunks per single buffer
		 */
		PoolAllocator(const size_t num_chunks);

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
		void * Allocate(const size_t allocation_size);

		/**
		 * Releases block of memory that was allocated previously
		 *
		 * @param[in] ptr Pointer to block of memory
		 */
		void Free(void * ptr);

	private:

		unsigned char* AllocateBuffer();
		
		size_t num_chunks_;
		size_t total_size_;
		size_t chunk_size_;
		size_t used_;
		StackLinkedList free_list_;
		std::vector<unsigned char*> buffers_;
	};

} // namespace scythe

#endif