#ifndef __SCYTHE_SPIN_LOCK_H__
#define __SCYTHE_SPIN_LOCK_H__

#include <atomic>

namespace scythe {

	/**
	 * Spinlock implementation with atomic.
	 * Three times faster than standard mutex.
	 */
	class Spinlock {
	public:
		/**
		 * Constructor
		 */
		Spinlock();

		/**
		 * Locks access on following segment of code
		 */
		void Lock();

		/**
		 * Tries to lock access on following segment of code
		 * @return True on success and false otherwise.
		 */
		bool TryLock();

		/**
		 * Unlocks access on previous segment of code
		 */
		void Unlock();

	private:

		/**
		 * Disallow copy
		 */
		Spinlock(const Spinlock &) = delete;

		/**
		 * Disallow assign
		 */
		Spinlock& operator =(const Spinlock &) = delete;

		std::atomic<bool> lock_ = {0};
	};

} // namespace scythe

#endif