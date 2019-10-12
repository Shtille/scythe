#ifndef __SCYTHE_STL_POOL_ALLOCATOR_H__
#define __SCYTHE_STL_POOL_ALLOCATOR_H__

#include "pool_allocator.h"

#include <limits>

namespace scythe {

	/**
	 * STL wrapper for pool allocator.
	 * The most effective allocator for following containers:
	 * - std::list
	 * - std::set
	 * - std::map
	 *
	 * @see PoolAllocator
	 */
	template <class T>
	class StlPoolAllocator {
	public:

		// Type definitions
		typedef T        value_type;
		typedef T*       pointer;
		typedef const T* const_pointer;
		typedef T&       reference;
		typedef const T& const_reference;
		typedef std::size_t    size_type;
		typedef std::ptrdiff_t difference_type;

		/**
		 * Structure to rebind allocator to another type
		 */
		template <class U>
		struct rebind {
		   typedef StlPoolAllocator<U> other;
		};

		/**
		 * Returns pointer for value reference
		 */
		pointer address(reference value) const
		{
		   return &value;
		}

		/**
		 * Returns constant pointer for constant value reference
		 */
		const_pointer address(const_reference value) const
		{
		   return &value;
		}

		/**
		 * Default constructor
		 */
		StlPoolAllocator() throw()
		: allocator_(40)
		{
		}

		/**
		 * Constructor
		 *
		 * @param[in] num_chunks Number of chunks per buffer
		 */
		StlPoolAllocator(std::size_t num_chunks) throw()
		: allocator_(num_chunks)
		{
		}

		/**
		 * Destructor
		 */
		~StlPoolAllocator() throw()
		{
		}

		/**
		 * Returns maximum number of elements that can be allocated
		 */
		size_type max_size() const throw()
		{
		   return std::numeric_limits<std::size_t>::max() / sizeof(T);
		}

		/**
		 * Allocates but doesn't initialize num elements of type T
		 */
		pointer allocate(size_type num, const void* = 0)
		{
		   size_type new_size = num * sizeof(T);
		   pointer ret = (pointer)allocator_.Allocate(new_size);
		   //pointer ret = (pointer)(::operator new(num*sizeof(T)));
		   return ret;
		}

		/**
		 * Initializes elements of allocated storage p with value value
		 */
		void construct(pointer p, const T& value)
		{
		   // Initialize memory with placement new
		   new((void*)p)T(value);
		}

		/**
		 * Destroys elements of initialized storage p
		 */
		void destroy(pointer p)
		{
		   // Destroy objects by calling their destructor
		   p->~T();
		}

		/**
		 * Deallocates storage p of deleted elements
		 */
		void deallocate(pointer p, size_type num)
		{
		   allocator_.Free((void*)p);
		   //::operator delete((void*)p);
		}

	private:

		PoolAllocator allocator_;
	};

	// Return that all specializations of this allocator are interchangeable
	template <class T1, class T2>
	bool operator == (const StlPoolAllocator<T1>&, const StlPoolAllocator<T2>&) throw()
	{
		return true;
	}
	template <class T1, class T2>
	bool operator != (const StlPoolAllocator<T1>&, const StlPoolAllocator<T2>&) throw()
	{
		return false;
	}

} // namespace scythe

#endif