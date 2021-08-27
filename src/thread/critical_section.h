#ifndef __SCYTHE_CRITICAL_SECTION_H__
#define __SCYTHE_CRITICAL_SECTION_H__

#include "common/platform.h"

namespace scythe {

	/**
	 * Platform independent critical section class
	 */
	class CriticalSection {
	public:

		/**
		 * Constructor
		 */
		CriticalSection();

		/**
		 * Destructor
		 */
		~CriticalSection();

		/**
		 * Locks access on following segment of code
		 */
		void Lock();

		/**
		 * Unlocks access on previous segment of code
		 */
		void Unlock();

	private:

		/**
		 * Disallow copy
		 */
		CriticalSection(const CriticalSection &) = delete;

		/**
		 * Disallow assign
		 */
		CriticalSection& operator =(const CriticalSection &) = delete;
	};

} // namespace scythe

#endif