#ifndef __SCYTHE_ALLOCATOR_H__
#define __SCYTHE_ALLOCATOR_H__

#include <cstddef>

namespace scythe {

	/**
	 * Allocator class interface
	 */
	class Allocator {
	public:

		/**
		 * Constructor
		 */
		Allocator() = default;

		/**
		 * Destructor
		 */
		virtual ~Allocator() = default;

		/**
		 * Allocates block of memory
		 *
		 * @param[in] allocation_size Size of memory block
		 */
		virtual void * Allocate(const size_t allocation_size) = 0;

		/**
		 * Releases block of memory that was allocated previously
		 *
		 * @param[in] ptr Pointer to block of memory
		 */
		virtual void Free(void * ptr) = 0;
	};

} // namespace scythe

#endif