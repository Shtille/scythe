#include "spin_lock.h"

#include <thread>

namespace scythe {

	Spinlock::Spinlock()
	: lock_(false)
	{
	}
	void Spinlock::Lock()
	{
		for (;;)
		{
			// Optimistically assume the lock is free on the first try
			if (!lock_.exchange(true, std::memory_order_acquire))
				return;
			// Wait for lock to be released without generating cache misses
			while (lock_.load(std::memory_order_relaxed))
			{
				// Issue X86 PAUSE or ARM YIELD instruction to reduce contention between
				// hyper-threads
				std::this_thread::yield();
			}
		}
	}
	bool Spinlock::TryLock()
	{
		// First do a relaxed load to check if lock is free in order to prevent
		// unnecessary cache misses if someone does while(!try_lock())
		return !lock_.load(std::memory_order_relaxed) &&
		       !lock_.exchange(true, std::memory_order_acquire);
	}
	void Spinlock::Unlock()
	{
		lock_.store(false, std::memory_order_release);
	}

} // namespace scythe