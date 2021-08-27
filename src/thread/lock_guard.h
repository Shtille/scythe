#ifndef __SCYTHE_LOCK_GUARD_H__
#define __SCYTHE_LOCK_GUARD_H__

namespace scythe {

	/**
	 * Scope lock guard class
	 */
	template <class Lock>
	class LockGuard {
	public:

		/**
		 * Constructor
		 */
		explicit LockGuard(Lock & lock)
		: lock_(lock)
		{
			lock_.Lock();
		}

		/**
		 * Destructor
		 */
		~LockGuard()
		{
			lock_.Unlock();
		}

	private:

		/**
		 * Disallow default constructor
		 */
		LockGuard() = delete;

		/**
		 * Disallow copy
		 */
		LockGuard(const LockGuard &) = delete;

		/**
		 * Disallow assign
		 */
		LockGuard& operator =(const LockGuard &) = delete;

		Lock & lock_;
	};

} // namespace scythe

#endif