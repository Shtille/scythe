#ifndef __SCYTHE_TEST_ALLOCATOR_H__
#define __SCYTHE_TEST_ALLOCATOR_H__

#include "allocator.h"

namespace scythe {

	/**
	 * Defines test allocator.
	 * Useful for testing allocation/deallocation balance during tests.
	 */
	class TestAllocator final : public Allocator {
	public:

		TestAllocator()
		: allocated_(0)
		{
		}

		/**
		 * Allocates block of memory
		 *
		 * @param[in] allocation_size Size of memory block
		 */
		void * Allocate(const size_t allocation_size) final
		{
			++allocated_;
			return reinterpret_cast<void*>(new unsigned char[allocation_size]);
		}

		/**
		 * Releases block of memory that was allocated previously
		 *
		 * @param[in] ptr Pointer to block of memory
		 */
		void Free(void * ptr) final
		{
			--allocated_;
			delete[] reinterpret_cast<unsigned char*>(ptr);
		}

		/**
		 * Returns current number of allocated blocks.
		 */
		int allocated() const { return allocated_; }

	private:
		int allocated_; //!< number of allocated blocks
	};

} // namespace scythe

#endif