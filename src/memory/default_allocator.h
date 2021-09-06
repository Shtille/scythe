#ifndef __SCYTHE_DEFAULT_ALLOCATOR_H__
#define __SCYTHE_DEFAULT_ALLOCATOR_H__

#include "allocator.h"

namespace scythe {

	/**
	 * Defines default allocator.
	 */
	class DefaultAllocator final : public Allocator {
	public:

		/**
		 * Allocates block of memory
		 *
		 * @param[in] allocation_size Size of memory block
		 */
		void * Allocate(const size_t allocation_size)
		{
			return reinterpret_cast<void*>(new unsigned char[allocation_size]);
		}

		/**
		 * Releases block of memory that was allocated previously
		 *
		 * @param[in] ptr Pointer to block of memory
		 */
		void Free(void * ptr) final
		{
			delete[] reinterpret_cast<unsigned char*>(ptr);
		}
	};

} // namespace scythe

#endif